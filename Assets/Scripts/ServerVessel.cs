using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

public class AdjacentTiles {

	private ServerVessel vessel;
	private Vec2i pos;

	public VesselTile l2Tile = null;
	public VesselTile lTile = null;
	public VesselTile blTile = null;
	public VesselTile bTile = null;
	public VesselTile b2Tile = null;
	public VesselTile brTile = null;
	public VesselTile b2rTile = null;
	public VesselTile br2Tile = null;
	public VesselTile tile = null;
	public VesselTile rTile = null;
	public VesselTile r2Tile = null;
	
	public AdjacentTiles (ServerVessel vessel) {
		this.vessel = vessel;
	}
	public AdjacentTiles (ServerVessel vessel, Vec2i start) {
		this.vessel = vessel;
		Reset(start);
	}

	public void Reset (Vec2i pos)
	{
		this.pos = pos;

		l2Tile = vessel.TryGetTile(new Vec2i(pos.x-2,pos.y));
		lTile = vessel.TryGetTile(new Vec2i(pos.x-1,pos.y));
		blTile = vessel.TryGetTile(new Vec2i(pos.x-1,pos.y-1));
		bTile = vessel.TryGetTile(new Vec2i(pos.x,pos.y-1));
		b2Tile = vessel.TryGetTile(new Vec2i(pos.x,pos.y-2));
		brTile = vessel.TryGetTile(new Vec2i(pos.x+1,pos.y-1));
		b2rTile = vessel.TryGetTile(new Vec2i(pos.x+1,pos.y-2));
		br2Tile = vessel.TryGetTile(new Vec2i(pos.x+2,pos.y-1));
		tile = vessel.TryGetTile(new Vec2i(pos.x, pos.y));
		rTile = vessel.TryGetTile(new Vec2i(pos.x+1, pos.y));
		r2Tile = vessel.TryGetTile(new Vec2i(pos.x+2, pos.y));
	}

	public void MoveRight () {

		VesselTile nextR2Tile = vessel.TryGetTile(new Vec2i(pos.x+3, pos.y));
		VesselTile nextBR2Tile = vessel.TryGetTile(new Vec2i(pos.x+3,pos.y-1));
		VesselTile nextB2RTile = vessel.TryGetTile(new Vec2i(pos.x+2,pos.y-2));

		blTile = bTile;
		bTile = brTile;
		brTile = br2Tile;
		br2Tile = nextBR2Tile;
		
		b2Tile = b2rTile;
		b2rTile = nextB2RTile;
		
		l2Tile = lTile;
		lTile = tile;
		tile = rTile;
		rTile = r2Tile;
		r2Tile = nextR2Tile;

		pos.x += 1;
	}
}

public class ServerVessel : Vessel
{
	public static float CHUNK_UPDATE_INTERVAL = 2.0f;

	private Vec2i bl = new Vec2i(0,0);
	private Vec2i tr = new Vec2i(0,0);
	private bool interiorExists;
	private Rect interiorSpace;
	private ulong spaceAddress;

	private List<VesselChunk> modifiedChunks = new List<VesselChunk>(5);
	public List<NetworkIdentity> netIdentities = new List<NetworkIdentity>();

	private List<Character2D> playersOnBoard = new List<Character2D>(5);

	private bool noPlayers;
	private float timeEmpty;
	private SimpleTimer chunkCheckTimer;


	private bool updateMessageBytes = true;
	private byte[] messageBytes;
	public byte[] MessageBytes
	{
		get
		{
			if (updateMessageBytes) {
				NetworkWriter nw = new NetworkWriter();
				
				nw.Write(interiorPosition);

				messageBytes = nw.ToArray();
				
				updateMessageBytes = false;
			}
			
			return messageBytes;
		}
	}

	public ServerVessel () :
		base(NextIndex++)
	{
		interiorExists = false;
		spaceAddress = 0;

		chunkCheckTimer.Set(CHUNK_UPDATE_INTERVAL);

		Vessels.Add(this);
	}
	
	public virtual void Update () {


		if (interiorExists) {
			
			if (chunkCheckTimer.Update()) {

				MarkAllChunksUnseen();
				
				for (int i = 0; i < playersOnBoard.Count; i++) {
					
					UpdatePlayer(playersOnBoard[i]);
					
				}
				
				DestroyUnseenChunks();

				chunkCheckTimer.Set(CHUNK_UPDATE_INTERVAL);
			}
		}
		
		if (noPlayers) {
			timeEmpty += Time.deltaTime;
		}
	}

	private void UpdatePlayer(Character2D player)
	{
		Vec2i pChunkI = WorldToChunkI(player.transform.position);
		
		if (pChunkI != player.ChunkI) {
			
			foreach (var chunk in chunks.TryGetOutsideInside(player.ChunkI, pChunkI, PLAYER_CHUNK_RANGE)) {
				
				if (chunk != null) {
					
					player.RpcSetChunk(Index, chunk.MessageBytes);
					
				}
				
			}
			player.ChunkI = pChunkI;
		}
		
		InstantiateNearbyChunks(player);
	}

	private void SendNearbyChunks(Character2D player)
	{
		foreach (var chunk in chunks.WithinRange(player.ChunkI, PLAYER_CHUNK_RANGE)) {

			if (chunk != null) {

				player.RpcSetChunk(Index, chunk.MessageBytes);

			}
		}
	}

	private void InstantiateNearbyChunks(Character2D player)
	{
		foreach (var chunk in chunks.WithinRange(player.ChunkI, PLAYER_CHUNK_RANGE)) {
			
			if (chunk != null) {
				
				chunk.Seen = true;
				
				if (!chunk.Instantiated) {
					
					InstantiateChunk(chunk);
					
				}
			}
		}
	}
	
	private void MarkAllChunksUnseen()
	{
		for (int i = 0; i < chunks.data.Length; i++) {
			if(chunks.data[i] != null)
				chunks.data[i].Seen = false;
		}
	}
	
	private void DestroyUnseenChunks()
	{
		for (int i = 0; i < chunks.data.Length; i++) {
			if(chunks.data[i] != null)
				if (!chunks.data[i].Seen && chunks.data[i].Instantiated)
					chunks.data[i].Destroy();
		}
	}
	
	private void AllocateInteriorSpace()
	{
		Vector2 interiorSize = new Vector2(
			(tr.x - bl.x) * (float)VesselChunk.SIZE, 
			(tr.y - bl.y) * (float)VesselChunk.SIZE);
		
		//pad interiorSize
		interiorSize *= 2.0f;
		
		spaceAddress = 0;
		interiorExists = SpaceAllocator.AllocateSpace(interiorSize, ref spaceAddress, ref interiorSpace);
		
		Vector2 localCenter = ((Vector2)bl + (Vector2)tr) * 0.5f * (float)VesselChunk.SIZE;
		
		interiorPosition = interiorSpace.center - localCenter;
		
		Debug.Log ("AllocateInteriorSpace: " + interiorSpace);
	}
	
	private VesselChunk CreateChunk(Vec2i index)
	{
		VesselChunk temp = new VesselChunk(index);
		chunks.Set(index.x, index.y, temp);
		
		if (index.x < bl.x){
			bl.x = index.x;
		}
		if (index.y < bl.y){
			bl.y = index.y;
		}
		
		if (index.x >= tr.x){
			tr.x = index.x + 1;
		}
		if (index.y >= tr.y){
			tr.y = index.y + 1;
		}
		
		return temp;
	}
	
	private void SendModifiedChunks()
	{
		for (int i = 0; i < modifiedChunks.Count; i++) {
			
			for (int j = 0; j < playersOnBoard.Count; j++) {
				
				Vec2i playerCI = WorldToChunkI(playersOnBoard[j].transform.position);
				Vec2i offset = modifiedChunks[i].Index - playerCI;
				if (offset <= PLAYER_CHUNK_RANGE) {
					
					playersOnBoard[j].RpcSetChunk(Index, modifiedChunks[i].MessageBytes);
					
				}
				
			}
			
		}
		
		modifiedChunks.Clear();
	}
	
	private void AddModifiedChunk(VesselChunk vc)
	{
		vc.Modified = true;

		if (modifiedChunks.Count != 0) {
			for (int i = 0; i < modifiedChunks.Count; i++) {
				if (modifiedChunks[i] == vc)
				{
					vc = modifiedChunks[0];
					modifiedChunks[0] = modifiedChunks[i];
					modifiedChunks[i] = vc;
					return;
				}
			}
			
			int j = modifiedChunks.Count;
			modifiedChunks.Add(vc);
			vc = modifiedChunks[0];
			modifiedChunks[0] = modifiedChunks[j];
			modifiedChunks[j] = vc;
		} else {
			modifiedChunks.Add(vc);
		}
	}
	
	public void SetTile(Vec2i index, VesselTile val)
	{
		Vec2i index2 = index;
		Vec2i chunkI = TileToChunkI(index);
		VesselChunk vc = chunks.TryGet(chunkI);

		if (vc == null) {
			vc = CreateChunk(chunkI);
		}

		try {
			index -= vc.OriginTileIndex();
			vc.SetTile(index, val);
			
			AddModifiedChunk(vc);
		} catch (System.Exception ex) {
			Debug.Log("Failed to set tile at " + index2.ToString() + "/" + index.ToString() + " at chunk " + chunkI.ToString());
		}
	}

	public void BuildWall(Vec2i index, int count, WallType type)
	{
		VesselTile tile;

		for (int i = 0; i < count; i++) {

			if (IsWallLegal(index, type)) {

				tile = TryGetTile(index);
				if (tile == null) {
					tile = new VesselTile();
					SetTile(index, tile);
				}

				tile.wallMask |= (byte)(1 << ((byte)type - 1));
				tile.wallNode = true;
				
				index += wallOffsets[(byte)type];

				tile = TryGetTile(index);
				if (tile == null) {
					tile = new VesselTile();
					SetTile(index, tile);
				}
				tile.wallNode = true;
			} else {
				index += wallOffsets[(byte)type];
			}
		}
	}

	private static void FillTile(AdjacentTiles t)
	{
		if ((t.tile.flags & (uint)VesselTile.FLAGS.SOLID_BLOCK) > 0) {
			t.tile.c0 = null;
			t.tile.c1 = null;
		}

		//take the compartments from left or bottom no matter what in these cases
		if ((t.bTile != null && t.bTile.Contains(WallTypeMask.OneByTwo)) || (t.brTile != null && t.brTile.Contains(WallTypeMask.OneByTwoFlipped))) {
			t.tile.c0 = t.bTile.c0.Instance;
			t.tile.c1 = t.bTile.c1.Instance;
			return;
		}
		if ((t.lTile != null && t.lTile.Contains(WallTypeMask.TwoByOne)) || (t.rTile != null && t.rTile.Contains(WallTypeMask.TwoByOneFlipped))) {
			t.tile.c0 = t.lTile.c0.Instance;
			t.tile.c1 = t.lTile.c1.Instance;
			return;
		}
		//

		//try take from bottom
		Compartment b = null;
		if (t.bTile != null) {
			if ((t.bTile.flags & (uint)VesselTile.FLAGS.SOLID_BLOCK) > 0)
				b = null;
			else if (!t.tile.Contains(WallTypeMask.OneByZero)) {
				//set the appropriate compartment field depending on how "bTile" is cut
				if (
					(t.blTile != null && t.blTile.Contains(WallTypeMask.TwoByOne)) ||
					(t.bTile.Contains(WallTypeMask.OneByOne)) ||
					(t.b2Tile != null && t.b2Tile.Contains(WallTypeMask.OneByTwo))) {
					b = t.bTile.c1.Instance;
				//} else if (
				//	(t.b2rTile != null && t.b2rTile.Contains(WallTypeMask.OneByTwoFlipped)) ||
				//	(t.brTile != null && t.brTile.Contains(WallTypeMask.OneByOneFlipped)) ||
				//	(t.br2Tile != null && t.br2Tile.Contains(WallTypeMask.TwoByOneFlipped))){
				//	b = t.bTile.c0.Instance;
				} else {
					b = t.bTile.c0.Instance;
				}
			}
		}
		Compartment l = null;
		if (t.lTile != null) {
			if ((t.lTile.flags & (uint)VesselTile.FLAGS.SOLID_BLOCK) > 0)
				l = null;
			else if (!t.tile.Contains(WallTypeMask.ZeroByOne)) {
				l = t.lTile.c0.Instance;
			}
		}

		if (
			(t.r2Tile != null && t.r2Tile.Contains(WallTypeMask.TwoByOneFlipped)) || 
			(t.rTile != null && t.rTile.Contains(WallTypeMask.OneByOneFlipped | WallTypeMask.OneByTwoFlipped))) {
			if (l != null && b != null) {
				l.Combine(b);
			}
			if (b != null) {
				t.tile.c1 = b;
			} else if (l != null) {
				t.tile.c1 = l;
			}

			t.tile.c0 = new Compartment();

			return;
		} else if (t.tile.Contains(WallTypeMask.TwoByOne | WallTypeMask.OneByOne | WallTypeMask.OneByTwo)) {
			if (b == null) {
				b = new Compartment();
			}
			t.tile.c0 = b;
			if (l == null) {
				l = new Compartment();
			}
			t.tile.c1 = l;
			return;
		} else {
			if (l != null && b != null) {
				l.Combine(b);
			}

			l = (l == null) ? b : l;
			if (l == null) {
				l = new Compartment();
			}
			t.tile.c0 = l.Instance;
		}
	}

	public void RebuildCompartments()
	{
		Vec2i start = ChunkIToTileI(bl);
		Vec2i end = ChunkIToTileI(tr);

		AdjacentTiles at = new AdjacentTiles(this);

		for (int i = start.y; i < end.y; i++) {

			at.Reset(new Vec2i(start.x, i));

			for (int j = start.x; j < end.x; j++) {
				if (at.tile != null) {

					FillTile(at);

				}
				at.MoveRight();
			}
		}

		return;
	}

	public Compartment CompartmentAt(Vector2 local)
	{
		Vec2i tileI = LocalToTile(local);
		Vector2 diff = local - TileToLocal(tileI);

		VesselTile oTile = TryGetTile(tileI);
		VesselTile tile = oTile;

		if (tile != null) {
			for (int i = (int)WallType.TwoByOne; i < (int)WallType.ZeroByOne; i++) {
				if (tile.Contains((WallType)i)) {
					if (Vector2.Dot(diff, new Vector2(-wallOffsets[i].y, wallOffsets[i].x)) > 0.0f) {
						return oTile.c1.Instance;
					} else {
						return oTile.c0.Instance;
					}
				}
			}

			Vec2i tempI = new Vec2i(tileI.x, tileI.y-1);
			if ((tile = TryGetTile(tempI)) != null && tile.Contains(WallType.OneByTwo)) {
				diff = local - TileToLocal(tempI);
				if (Vector2.Dot(diff, new Vector2(-wallOffsets[(int)WallType.OneByTwo].y, wallOffsets[(int)WallType.OneByTwo].x)) > 0.0f) {
					return oTile.c1.Instance;
				} else {
					return oTile.c0.Instance;
				}
			} 
			tempI = new Vec2i(tileI.x+1, tileI.y-1);
			if (((tile = TryGetTile(tempI))) != null && tile.Contains(WallType.OneByTwoFlipped)) {
				diff = local - TileToLocal(tempI);
				if (Vector2.Dot(diff, new Vector2(-wallOffsets[(int)WallType.OneByTwoFlipped].y, wallOffsets[(int)WallType.OneByTwoFlipped].x)) > 0.0f) {
					return oTile.c1.Instance;
				} else {
					return oTile.c0.Instance;
				}
			}
			tempI = new Vec2i(tileI.x+1, tileI.y);
			if ((tile = TryGetTile(tempI)) != null && tile.Contains(WallType.OneByOneFlipped)) {
				diff = local - TileToLocal(tempI);
				if (Vector2.Dot(diff, new Vector2(-wallOffsets[(int)WallType.OneByOneFlipped].y, wallOffsets[(int)WallType.OneByOneFlipped].x)) > 0.0f) {
					return oTile.c1.Instance;
				} else {
					return oTile.c0.Instance;
				}
			}
			
			tempI = new Vec2i(tileI.x-1, tileI.y);
			if ((tile = TryGetTile(tempI)) != null && tile.Contains(WallType.TwoByOne)) {
				diff = local - TileToLocal(tempI);
				if (Vector2.Dot(diff, new Vector2(-wallOffsets[(int)WallType.TwoByOne].y, wallOffsets[(int)WallType.TwoByOne].x)) > 0.0f) {
					return oTile.c1.Instance;
				} else {
					return oTile.c0.Instance;
				}
			}

			for (int i = 1; i < 3; i++) {
				tempI = new Vec2i(tileI.x+i, tileI.y);
				if ((tile = TryGetTile(tempI)) != null && tile.Contains(WallType.TwoByOneFlipped)) {
					diff = local - TileToLocal(tempI);
					if (Vector2.Dot(diff, new Vector2(-wallOffsets[(int)WallType.TwoByOneFlipped].y, wallOffsets[(int)WallType.TwoByOneFlipped].x)) > 0.0f) {
						return oTile.c1.Instance;
					} else {
						return oTile.c0.Instance;
					}
				}
			}

			return oTile.c0.Instance;
		}

		return null;
	}

	public void SetCompartmentFloor(FloorType type, Compartment c)
	{
		Vec2i start = ChunkIToTileI(bl);
		Vec2i end = ChunkIToTileI(tr);

		for (int i = start.y; i < end.y; i++) {
			for (int j = start.x; j < end.x; j++) {
				Vec2i tileI = new Vec2i(j, i);
				VesselTile tile;
				if ((tile = TryGetTile(tileI)) != null) {
					if (tile.c0 != null && tile.c0.Instance == c) {
						tile.floor0 = type;
						Vec2i chunkI = TileToChunkI(tileI);
						AddModifiedChunk(chunks.Get(chunkI));
					}
					if (tile.c1 != null && tile.c1.Instance == c) {
						tile.floor1 = type;
						Vec2i chunkI = TileToChunkI(tileI);
						AddModifiedChunk(chunks.Get(chunkI));
					}
				}
			}
		}

		SendModifiedChunks();
	}

	public virtual bool PlaceBlock(BlockType type, Vec2i location)
	{
		VesselTile tile;

		if ((tile = TryGetTile(location)) != null) {
			tile.blockT = type;
		}

		return false;
	}
	
	public virtual void BuildFoundation(Vec2i origin, Vec2i size)
	{
		VesselTile tile;

		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {

				Vec2i temp = origin + new Vec2i(j, i);
				if ((tile = TryGetTile(temp)) == null) {
					SetTile(temp, new VesselTile());
				}
			}
		}
	}

	public void AddPlayer(Character2D player, Vector2 position)
	{
		if (!interiorExists) {
			AllocateInteriorSpace();
		}
		
		player.transform.position = (Vector3)(interiorPosition + position);

		netIdentities.Add(player.networkIdentity);
		player.currentVessel = this;
		playersOnBoard.Add(player);
		player.RpcSyncVessel(Index, MessageBytes);
		player.ChunkI = WorldToChunkI(player.transform.position);
		SendNearbyChunks(player);
		InstantiateNearbyChunks(player);

		noPlayers = false;
		timeEmpty = 0.0f;
	}
	
	public void RemovePlayer(Character2D player)
	{
		playersOnBoard.Remove(player);
		netIdentities.Remove(player.networkIdentity);
		player.currentVessel = null;
		if (playersOnBoard.Count == 0) {
			noPlayers = true;
		}
	}

	public static QTSpaceAllocator SpaceAllocator = new QTSpaceAllocator(8);
	public static List<ServerVessel> Vessels = new List<ServerVessel>(512);

	private static uint nextIndex = 0;
	private static uint NextIndex
	{
		get{
			return nextIndex;
		}
		set{
			nextIndex = value;
		}
	}
}


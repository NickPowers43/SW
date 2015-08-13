using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

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
	
	public void RebuildCompartments()
	{
		Vec2i start = ChunkIToTileI(bl);
		Vec2i end = ChunkIToTileI(tr + 1);

		for (int i = start.y; i < end.y; i++) {

			VesselTile lTile = null;
			VesselTile tile = TryGetTile(new Vec2i(start.x, i));
			VesselTile rTile = TryGetTile(new Vec2i(start.x+1, i));
			//Compartment c0 = null;
			//Compartment c1 = null;//impossible for this one to be non-null only

			for (int j = start.x; j < end.x; j++) {
				VesselTile nextRTile = TryGetTile(new Vec2i(j+2, i));

				Vec2i tileI = new Vec2i(j,i);
				if (tile != null) {

					VesselTile r2Tile = TryGetTile(new Vec2i(tileI.x+2,tileI.y));
					VesselTile bTile = TryGetTile(new Vec2i(tileI.x,tileI.y-1));
					VesselTile brTile = TryGetTile(new Vec2i(tileI.x+1,tileI.y-1));


					if (tile.Contains(WallTypeMask.ZeroByOne)) {
						if (tile.Contains(WallTypeMask.OneByZero)) {
							tile.c0 = new Compartment();
							tile.c1 = null;
						} else {
							//try take bottom's compartment
							if (bTile != null) {

							}
						}

					} else if (lTile != null) {
						if (tile.Contains(WallTypeMask.TwoByOne | WallTypeMask.OneByTwo | WallTypeMask.OneByOne)) { //this tile contains a non-ortho wall originating from origin
							tile.c1 = lTile.c0;
							tile.c0 = new Compartment();//or take bottom's
						} else if (lTile.Contains(WallTypeMask.TwoByOne)) { //left tile contains a TwoByOne
						} else if (bTile != null && bTile.Contains(WallTypeMask.OneByTwo)) { //bottom tile contains a OneByTwo
						} else if (brTile != null && brTile.Contains(WallTypeMask.OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
						} else if (rTile != null && rTile.Contains(WallTypeMask.TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
							tile.c0 = lTile.c0;
							tile.c1 = lTile.c1;//or take bottom's
						} else if (rTile != null && rTile.Contains(WallTypeMask.OneByOneFlipped)) { //r tile contains a OneByOneFlipped
						} else if (rTile != null && rTile.Contains(WallTypeMask.OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
						} else if (r2Tile != null && r2Tile.Contains(WallTypeMask.TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
						} else {
							if (tile.Contains(WallTypeMask.ZeroByOne)) {
								tile.c0 = new Compartment();
							} else {
								tile.c0 = lTile.c0;
							}
						}
					} else {
						if (tile.Contains(WallTypeMask.TwoByOne | WallTypeMask.OneByTwo | WallTypeMask.OneByOne)) { //this tile contains a non-ortho wall originating from origin
							tile.c1 = null;
							tile.c0 = new Compartment();//or take bottom's
						} else if (bTile != null && bTile.Contains(WallTypeMask.OneByTwo)) { //bottom tile contains a OneByTwo
							tile.c0 = bTile.c0;
							tile.c1 = bTile.c1;
						} else if (brTile != null && brTile.Contains(WallTypeMask.OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
							tile.c0 = bTile.c0;
							tile.c1 = bTile.c1;
						} else if (rTile != null && rTile.Contains(WallTypeMask.TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
							tile.c0 = new Compartment();
							tile.c1 = new Compartment();//or take bottom's
						} else if (rTile != null && rTile.Contains(WallTypeMask.OneByOneFlipped)) { //r tile contains a OneByOneFlipped
							tile.c0 = new Compartment();
							tile.c1 = new Compartment();//or take bottom's
						} else if (rTile != null && rTile.Contains(WallTypeMask.OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
							tile.c0 = new Compartment();
							tile.c1 = new Compartment();//or take bottom's
						} else if (r2Tile != null && r2Tile.Contains(WallTypeMask.TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
							tile.c0 = new Compartment();
							tile.c1 = new Compartment();//or take bottom's
						} else {
							tile.c0 = new Compartment();//or take bottom's
							tile.c1 = null;
						}
					}

						if (c0 != null && c1 != null) {
							tile.c0 = c0;
							tile.c1 = c1;

							
						} else if (c0 != null) {
							
							if (tile.Contains(WallTypeMask.ZeroByOne)) {
								c0 = new Compartment();
								tile.c0 = c0;
								tile.c1 = null;
							} else if (tile.Contains(WallTypeMask.TwoByOne)) { //this tile contains a TwoByOne
								c1 = c0;
								c0 = new Compartment();
								tile.c0 = c0;
								tile.c1 = c1;
							} else if (tile.Contains(WallTypeMask.OneByTwo | WallTypeMask.OneByOne)) { //this tile contains a OneByTwo
								tile.c0 = c0;
								tile.c1 = c1;
								c0 = null;
								c1 = null;
							}
							
						} else {
							
							//no compartments we can use from the other tile
							
						}
					}


					if (lTile == null) {
						//create new compartment for this tile
					} else {
						//take left tile compartment if you can
					}




				}

				lTile = tile;
				tile = rTile;
				rTile = nextRTile;
			}
		}
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


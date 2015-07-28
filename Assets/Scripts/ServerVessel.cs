using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

public class ServerVessel : Vessel
{
	private Vec2i bl = new Vec2i(0,0);
	private Vec2i tr = new Vec2i(0,0);
	private bool interiorExists;
	private Rect interiorSpace;
	private ulong spaceAddress;

	private List<VesselChunk> modifiedChunks = new List<VesselChunk>(5);
	private List<Character2D> playersOnBoard = new List<Character2D>(5);

	private bool noPlayers;
	private float timeEmpty;

	public ServerVessel () :
		base()
	{
		interiorExists = false;
		spaceAddress = 0;
	}
	
	public virtual void Update () {
		
		if (interiorExists) {
			
			MarkAllChunksUnseen();
			
			for (int i = 0; i < playersOnBoard.Count; i++) {
				
				Character2D player = playersOnBoard[i];
				
				Vec2i pChunkI = WorldToChunkI(player.transform.position);
				
				if (pChunkI != player.ChunkI) {
					
					foreach (var chunk in chunks.TryGetOutsideInside(player.ChunkI, pChunkI, PLAYER_CHUNK_RANGE)) {
						
						player.RpcCreateChunk(chunk.MessageBytes);
						
					}
					player.ChunkI = pChunkI;
				}
				
				foreach (var chunk in chunks.WithinRange(player.ChunkI, PLAYER_CHUNK_RANGE)) {
					
					if (chunk != null) {
						
						chunk.Seen = true;
						
						if (!chunk.Instantiated) {
							VesselChunk top = chunks.TryGet(chunk.Index.x,chunk.Index.y+1);
							VesselChunk left = chunks.TryGet(chunk.Index.x-1,chunk.Index.y);
							VesselChunk right = chunks.TryGet(chunk.Index.x+1,chunk.Index.y);
							VesselChunk bottom = chunks.TryGet(chunk.Index.x,chunk.Index.y-1);
							VesselChunk bottomRight = chunks.TryGet(chunk.Index.x+1,chunk.Index.y-1);
							
							Vector2 chunkOffset = new Vector2(
								chunk.Index.x * (float)VesselChunk.SIZE,
								chunk.Index.y * (float)VesselChunk.SIZE);
							
							chunk.Instantiate(top, left, right, bottom, bottomRight, chunkOffset);
						}
					}
					
				}
			}
			
			DestroyUnseenChunks();
			
		}
		
		if (noPlayers) {
			timeEmpty += Time.deltaTime;
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
					
					playersOnBoard[j].RpcCreateChunk(modifiedChunks[i].MessageBytes);
					
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
		Vec2i chunkI = TileToChunkI(index);
		index -= chunkI << VesselChunk.SIZE_POW;
		
		VesselChunk vc = chunks.TryGet(chunkI);
		
		if (vc == null) {
			vc = CreateChunk(chunkI);
		}
		
		vc.SetTile(index, val);
		
		AddModifiedChunk(vc);
	}
	
	public void AddPlayer(Character2D player)
	{
		if (!interiorExists) {
			AllocateInteriorSpace();
		}
		
		//find a spawn point for the player
		Vector2 position = Vector2.zero;
		
		player.transform.position = (Vector3)(interiorPosition + position);
		playersOnBoard.Add(player);
		noPlayers = false;
		timeEmpty = 0.0f;
	}
	
	public void AddPlayer(Character2D player, Vector2 position)
	{
		if (!interiorExists) {
			AllocateInteriorSpace();
		}
		
		player.transform.position = (Vector3)(interiorPosition + position);
		playersOnBoard.Add(player);
		noPlayers = false;
		timeEmpty = 0.0f;
	}
	
	public void RemovePlayer(Character2D player)
	{
		playersOnBoard.Remove(player);
		if (playersOnBoard.Count == 0) {
			noPlayers = true;
		}
	}
	
	private static Dictionary<Vessel, List<NetworkIdentity>> vesselIdentities = new Dictionary<Vessel, List<NetworkIdentity>>();
	public static Dictionary<Vessel, List<NetworkIdentity>> VesselIdentities
	{
		get{
			return vesselIdentities;
		}
		set{
			vesselIdentities = value;
		}
	}
	
	private static QTSpaceAllocator spaceAllocator = new QTSpaceAllocator(13);
	public static QTSpaceAllocator SpaceAllocator
	{
		get{
			return spaceAllocator;
		}
		set{
			spaceAllocator = value;
		}
	}
	
	private static List<Vessel> activeVessels = new List<Vessel>(512);
	public static List<Vessel> ActiveVessels
	{
		get{
			return activeVessels;
		}
		set{
			activeVessels = value;
		}
	}
}


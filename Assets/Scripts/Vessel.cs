using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

public class Vessel {

	public static int PLAYER_CHUNK_RANGE = 2;

	protected Vector2 interiorPosition;//location of chunkI(0,0) in world coordinates

	protected DynamicArray2D<VesselChunk> chunks = new DynamicArray2D<VesselChunk>();

	public readonly uint Index;

	public Vessel(uint index)
	{
		this.Index = index;
	}

	public Vector2 WorldToLocal(Vector2 world)
	{
		return world - interiorPosition;
	}

	public Vec2i WorldToChunkI(Vector2 world)
	{
		return ChunkI(WorldToLocal(world));
	}
	
	public static Vector2 ChunkIToLocal(Vec2i chunkI)
	{
		return new Vector2(
			chunkI.x * (float)VesselChunk.SIZE,
			chunkI.y * (float)VesselChunk.SIZE);
	}
	
	public Vec2i ChunkI(Vector2 localPosition)
	{
		return new Vec2i(
			(localPosition.x < 0.0f) ? -1 - (int)(localPosition.x / -(float)VesselChunk.SIZE) : (int)(localPosition.x / (float)VesselChunk.SIZE),
			(localPosition.y < 0.0f) ? -1 - (int)(localPosition.y / -(float)VesselChunk.SIZE) : (int)(localPosition.y / (float)VesselChunk.SIZE));
	}
	
	public static Vec2i TileToChunkI(Vec2i tileI)
	{
		Vec2i output;
		output.x = (tileI.x >= 0) ? tileI.x >> VesselChunk.SIZE_POW : (tileI.x >> VesselChunk.SIZE_POW) - 1;
		output.y = (tileI.y >= 0) ? tileI.y >> VesselChunk.SIZE_POW : (tileI.y >> VesselChunk.SIZE_POW) - 1;
		return output;
	}
	
	public static Vec2i TileOffset(Vec2i tileI, Vec2i chunkI)
	{
		Vec2i output;
		output.x = (tileI.x >= 0) ? tileI.x - (chunkI.x << VesselChunk.SIZE_POW) : tileI.x - (chunkI.x << VesselChunk.SIZE_POW);
		output.y = (tileI.y >= 0) ? tileI.y - (chunkI.y << VesselChunk.SIZE_POW) : tileI.y - (chunkI.y << VesselChunk.SIZE_POW);
		return output;
	}
	
	protected void InstantiateChunk(VesselChunk chunk)
	{
		VesselChunk top = chunks.TryGet(chunk.Index.x,chunk.Index.y+1);
		VesselChunk left = chunks.TryGet(chunk.Index.x-1,chunk.Index.y);
		VesselChunk right = chunks.TryGet(chunk.Index.x+1,chunk.Index.y);
		VesselChunk bottom = chunks.TryGet(chunk.Index.x,chunk.Index.y-1);
		VesselChunk bottomRight = chunks.TryGet(chunk.Index.x+1,chunk.Index.y-1);
		Vector2 chunkOffset = ChunkIToLocal(chunk.Index);
		chunk.Instantiate(top, left, right, bottom, bottomRight, chunkOffset + interiorPosition);
	}
}

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

	public Vessel()
	{

	}

	public Vector2 WorldToLocal(Vector2 world)
	{
		return world - interiorPosition;
	}

	public Vec2i WorldToChunkI(Vector2 world)
	{
		return ChunkI(WorldToLocal(world));
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

}

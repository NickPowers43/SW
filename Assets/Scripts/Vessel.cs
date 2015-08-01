using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

public enum WallType : byte
{
	None = 0,
	OneByZero = 1,
	TwoByOne = 2,
	OneByOne = 3,
	OneByTwo = 4,
	ZeroByOne = 5,
	OneByTwoFlipped = 6,
	OneByOneFlipped = 7,
	TwoByOneFlipped = 8,
}
public enum FloorType : byte
{
	None = 0,
	Basic = 1
}

public class Vessel {

	public static Vec2i[] wallOffsets = new Vec2i[9] {
		new Vec2i(0,0),
		new Vec2i(1,0),
		new Vec2i(2,1),
		new Vec2i(1,1),
		new Vec2i(1,2),
		new Vec2i(0,1),
		new Vec2i(-1,2),
		new Vec2i(-1,1),
		new Vec2i(-2,1)
	};
	public static int PLAYER_CHUNK_RANGE = 2;

	protected Vector2 interiorPosition;//location of chunkI(0,0) in world coordinates

	protected DynamicArray2D<VesselChunk> chunks = new DynamicArray2D<VesselChunk>();

	public readonly uint Index;

	public Vessel(uint index)
	{
		this.Index = index;
	}
	
	public bool IsWallLegal(Vec2i index, WallType type)
	{
		if (type != WallType.OneByZero && type != WallType.ZeroByOne) {

			int hDir = (type < WallType.ZeroByOne) ? 1 : -1;

			if (ContainsWall(new Vec2i(index.x,index.y+1)) || ContainsWall(new Vec2i(index.x+hDir,index.y))) {
				Debug.Log("Vessel:59");
				return false;
			}

			int diff = Mathf.Abs((byte)type - (byte)WallType.ZeroByOne);

			if (diff != 2) {
				if (ContainsWall(new Vec2i(index.x+hDir,index.y+1))) {
					Debug.Log("Vessel:67");
					return false;
				}
				if (diff == 1) {
					if (ContainsWall(new Vec2i(index.x,index.y+2))) {
						Debug.Log("Vessel:72");
						return false;
					}
				} else if (diff == 3) {
					if (ContainsWall(new Vec2i(index.x+hDir+hDir,index.y))) {
						Debug.Log("Vessel:77");
						return false;
					}
				}
			}

		}

		return LegalWallStart(type, index) && LegalWallEnd(type, index + wallOffsets[(byte)type]);
	}

	public bool ContainsWall(Vec2i index)
	{
		VesselTile tile = TryGetTile(index);
		
		if (tile != null) {
			return tile.wallNode;
//			if (tile.wall0T != WallType.None) {
//				return true;
//			}
		}

		//check other walls that touch index
//		for (byte i = 1; i < 9; i++) {
//			VesselTile otherTile = TryGetTile(index - wallOffsets[i]);
//			if (otherTile != null) {
//				if (otherTile.Contains(i)) {
//					return true;
//				}
//			}
//		}

		return false;
	}

	public bool LegalWallStart(WallType type, Vec2i index)
	{
		VesselTile tile = TryGetTile(index);
		
		if (tile != null) {

			if (tile.wall1T != WallType.None)
				Debug.Log("Vessel:119");
				return false;
			if (tile.wall0T != WallType.None) {
				if (Mathf.Abs((byte)tile.wall0T - (byte)type) < 4) {
					Debug.Log("Vessel:123");
					return false;
				}
			}
		}

		for (byte i = 1; i < 9; i++) {
			VesselTile otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != null) {
				if (otherTile.Contains(i)) {
					if (!NonAcuteSequence((WallType)i, type)) {
						Debug.Log("Vessel:134");
						return false;
					}
				}
			}
		}
		
		return true;
	}

	public bool LegalWallEnd(WallType type, Vec2i index)
	{
		VesselTile tile = TryGetTile(index);

		if (tile != null) {
			//check with the walls originating from index
			if (tile.wall0T != WallType.None) {
				if (!NonAcuteSequence(type, tile.wall0T)) {
					Debug.Log("Vessel:152");
					return false;
				}
				if (tile.wall1T != WallType.None) {
					if (!NonAcuteSequence(type, tile.wall1T)) {
						Debug.Log("Vessel:157");
						return false;
					}
				}
			}
		}

		//check other walls that touch index
		for (byte i = 1; i < 9; i++) {
			VesselTile otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != null) {
				if (otherTile.Contains(i)) {
					if (Mathf.Abs((byte)type - (byte)i) < 4) {
						Debug.Log("Vessel:170");
						return false;
					}
				}
			}
		}

		return true;
	}

	public bool NonAcuteSequence(WallType wall0, WallType wall1)
	{
		if (wall1 < WallType.ZeroByOne) {
			if ((byte)wall0 > 4 + (byte)wall1) {
				Debug.Log("Vessel:184");
				return false;
			}
		} else if (wall1 > WallType.ZeroByOne) {
			if ((byte)wall0 < (byte)wall1 - 4) {
				Debug.Log("Vessel:189");
				return false;
			}
		}

		return true;
	}
	
	public VesselTile TryGetTile(Vec2i index)
	{
		Vec2i chunkI = TileToChunkI(index);
		VesselChunk vc = chunks.TryGet(chunkI);
		
		if (vc == null) {
			return null;
		} else {
			index = index - (chunkI << VesselChunk.SIZE_POW);
			return vc.TileAt(index);
		}
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

	public VesselChunk Top(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x, chunk.Index.y+1); }
	public VesselChunk Bottom(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x, chunk.Index.y-1); }
	public VesselChunk Left(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x-1, chunk.Index.y); }
	public VesselChunk Right(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x+1, chunk.Index.y); }
	public VesselChunk TopLeft(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x-1, chunk.Index.y+1); }
	public VesselChunk TopRight(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x+1, chunk.Index.y+1); }
	public VesselChunk BottomLeft(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x-1, chunk.Index.y-1); }
	public VesselChunk BottomRight(VesselChunk chunk) { return chunks.TryGet(chunk.Index.x+1, chunk.Index.y-1); }
	
	protected void InstantiateChunk(VesselChunk chunk)
	{
		Vector2 chunkOffset = ChunkIToLocal(chunk.Index);
		chunk.Instantiate(
			Top(chunk), 
			Left(chunk), 
			Right(chunk), 
			Bottom(chunk), 
			BottomRight(chunk), 
			chunkOffset + interiorPosition);
	}
}

using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;

public enum BlockType : byte
{
	None = 0,
	Spawner = 1,
	Turret = 2
}

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

public enum WallTypeMask : byte
{
	None = 0,
	OneByZero = 1,
	TwoByOne = 2,
	OneByOne = 4,
	OneByTwo = 8,
	ZeroByOne = 16,
	OneByTwoFlipped = 32,
	OneByOneFlipped = 64,
	TwoByOneFlipped = 128,
}

public enum FloorType : byte
{
	None = 0,
	Basic = 1,
	SmoothWhite = 2,
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
		VesselTile tile = null;
		if ((tile = TryGetTile(index)) != null && tile.blockT != BlockType.None) {
			return false;
		}
		if ((tile = TryGetTile(new Vec2i(index.x-1, index.y))) != null  && tile.blockT != BlockType.None) {
			return false;
		}

		//check if foundations have been set and walls nodes are not too close
		//to the new wall
		if (!(type == WallType.OneByZero || type == WallType.ZeroByOne)) {

			int hDir = (type < WallType.ZeroByOne) ? 1 : -1;
			int diff = Mathf.Abs((byte)type - (byte)WallType.ZeroByOne);

			if (hDir < 0) {
				if ((tile = TryGetTile(new Vec2i(index.x-1, index.y))) == null) {
					return false;
				} else {
					if (tile.blockT != BlockType.None)
						return false;
				}
				if (((tile = TryGetTile(new Vec2i(index.x-1, index.y-1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x-2, index.y))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x-1, index.y+1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x, index.y))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x-2, index.y-1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x, index.y+1))) != null && tile.blockT != BlockType.None)) {
					return false;
				}
				if (diff != 2) {
					if (((tile = TryGetTile(new Vec2i(index.x-1, index.y+1))) != null && tile.blockT != BlockType.None) ||
					     ContainsWall(new Vec2i(index.x-1,index.y+1))) {
						return false;
					}
				}
				if (diff == 3) {
					if (TryGetTile(new Vec2i(index.x-2, index.y)) == null ||
					    ContainsWall(new Vec2i(index.x-2,index.y))) {
						return false;
					}
					if (((tile = TryGetTile(new Vec2i(index.x-3, index.y-1))) != null && tile.blockT != BlockType.None) ||
					    ((tile = TryGetTile(new Vec2i(index.x-3, index.y))) != null && tile.blockT != BlockType.None)) {
						return false;
					}
				} else if (diff == 1) {
					if (TryGetTile(new Vec2i(index.x-1, index.y+1)) == null ||
					    ContainsWall(new Vec2i(index.x,index.y+2))) {
						return false;
					}
					if (((tile = TryGetTile(new Vec2i(index.x-2, index.y+2))) != null && tile.blockT != BlockType.None) ||
					    ((tile = TryGetTile(new Vec2i(index.x-1, index.y+2))) != null && tile.blockT != BlockType.None)) {
						return false;
					}
				}
			} else {
				if ((tile = TryGetTile(new Vec2i(index.x, index.y))) == null) {
					return false;
				} else {
					if (tile.blockT != BlockType.None)
						return false;
				}
				if ((tile = TryGetTile(new Vec2i(index.x, index.y-1))) != null && tile.blockT != BlockType.None) {
					return false;
				}
				if (((tile = TryGetTile(new Vec2i(index.x, index.y-1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x+1, index.y))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x, index.y+1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x-1, index.y))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x+1, index.y-1))) != null && tile.blockT != BlockType.None) ||
				    ((tile = TryGetTile(new Vec2i(index.x-1, index.y+1))) != null && tile.blockT != BlockType.None)) {
					return false;
				}
				if (diff != 2) {
					if (((tile = TryGetTile(new Vec2i(index.x+1, index.y+1))) != null && tile.blockT != BlockType.None) ||
					    ContainsWall(new Vec2i(index.x+1,index.y+1))) {
						return false;
					}
				}
				if (diff == 3) {
					if (TryGetTile(new Vec2i(index.x+1, index.y)) == null ||
					    ContainsWall(new Vec2i(index.x+2,index.y))) {
						return false;
					}
					if (((tile = TryGetTile(new Vec2i(index.x+2, index.y-1))) != null && tile.blockT != BlockType.None) ||
					    ((tile = TryGetTile(new Vec2i(index.x+2, index.y))) != null && tile.blockT != BlockType.None)) {
						return false;
					}
				} else if (diff == 1) {
					if (TryGetTile(new Vec2i(index.x, index.y+1)) == null ||
					    ContainsWall(new Vec2i(index.x,index.y+2))) {
						return false;
					}
					if (((tile = TryGetTile(new Vec2i(index.x-1, index.y+2))) != null && tile.blockT != BlockType.None) ||
					    ((tile = TryGetTile(new Vec2i(index.x, index.y+2))) != null && tile.blockT != BlockType.None)) {
						return false;
					}
				}
			}

			
			if (ContainsWall(new Vec2i(index.x,index.y+1)) || ContainsWall(new Vec2i(index.x+hDir,index.y))) {
				return false;
			}
		} else {
			if (TryGetTile(index) == null) {
				return false;
			}
		}

		Vec2i end = index + wallOffsets[(byte)type];

		if (TooCloseToWall(end) || TooCloseToWall(index)) {
			return false;
		}

		return LegalWallStart(type, index) && LegalWallEnd(type, end);
	}

	public bool ContainsWall(Vec2i index)
	{
		VesselTile tile = TryGetTile(index);
		
		if (tile != null) {
			if (tile.wallNode) {
				return true;
			} else {
				return false;
			}
		}

		return false;
	}

	public bool LegalWallStart(WallType type, Vec2i index)
	{
		VesselTile tile = TryGetTile(index);
		
		if (tile != null) {

			WallType wall0;
			WallType wall1;
			int wallCount = tile.GetWalls(out wall0, out wall1);

			if (wallCount > 1) {
				return false;
			}
			if (wallCount == 1) {
				if (Mathf.Abs((byte)wall0 - (byte)type) < 4) {
					return false;
				}
			}
		}

		for (byte i = 1; i < 9; i++) {
			VesselTile otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != null) {
				if (otherTile.Contains((WallType)i)) {
					if (!NonAcuteSequence((WallType)i, type)) {
						return false;
					}
				}
			}
		}

		return true;
	}

	public bool TooCloseToWall(Vec2i index)
	{
		VesselTile tile;

		//0
		tile = TryGetTile(new Vec2i(index.x-1, index.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOne | WallTypeMask.TwoByOne | WallTypeMask.OneByTwo)) {
				return true;
			}
		}
		//1
		tile = TryGetTile(new Vec2i(index.x, index.y-1));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOne | 
				WallTypeMask.OneByTwo | 
				WallTypeMask.OneByOneFlipped | 
				WallTypeMask.OneByTwoFlipped | 
				WallTypeMask.TwoByOne | 
				WallTypeMask.TwoByOneFlipped)) {
				return true;
			}
		}
		//2
		tile = TryGetTile(new Vec2i(index.x+1, index.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOneFlipped | WallTypeMask.TwoByOneFlipped | WallTypeMask.OneByTwoFlipped)) {
				return true;
			}
		}
		//3
		tile = TryGetTile(new Vec2i(index.x-1, index.y-1));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByTwo | WallTypeMask.TwoByOne)) {
				return true;
			}
		}
		//4
		tile = TryGetTile(new Vec2i(index.x+1, index.y-1));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOneFlipped | WallTypeMask.OneByTwoFlipped)) {
				return true;
			}
		}
		//5
		tile = TryGetTile(new Vec2i(index.x-2, index.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.TwoByOne)) {
				return true;
			}
		}
		//6
		tile = TryGetTile(new Vec2i(index.x+2, index.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.TwoByOneFlipped)) {
				return true;
			}
		}
		//7
		tile = TryGetTile(new Vec2i(index.x, index.y-2));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByTwoFlipped | WallTypeMask.OneByTwo)) {
				return true;
			}
		}

		return false;
	}

	public bool EmptyRect(Vec2i bl, Vec2i size)
	{
		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {

				Vec2i index = bl + new Vec2i(j, i);

				VesselTile tile = TryGetTile(index);

				if (tile != null) {
					if (!EmptyTile(index) ||
						((i != 0) && tile.Contains(WallTypeMask.OneByZero)) ||
						((j != 0) && tile.Contains(WallTypeMask.ZeroByOne))) {
						return false;
					}
				} else {
					if (!EmptyTile(index)) {
						return false;
					}
				}


			}
		}

		return true;
	}
	
	public bool EmptyTile(Vec2i loc)
	{
		VesselTile tile;

		tile = TryGetTile(loc);
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOne | WallTypeMask.TwoByOne | WallTypeMask.OneByTwo)) {
				return false;
			}
		}
		tile = TryGetTile(new Vec2i(loc.x-1,loc.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.TwoByOne)) {
				return false;
			}
		}
		tile = TryGetTile(new Vec2i(loc.x+1,loc.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByOneFlipped | WallTypeMask.TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGetTile(new Vec2i(loc.x+2,loc.y));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGetTile(new Vec2i(loc.x,loc.y-1));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByTwo)) {
				return false;
			}
		}
		tile = TryGetTile(new Vec2i(loc.x+1,loc.y-1));
		if (tile != null) {
			if (tile.Contains(WallTypeMask.OneByTwoFlipped)) {
				return false;
			}
		}
		
		return true;
	}

	public bool LegalWallEnd(WallType type, Vec2i index)
	{
		VesselTile tile = TryGetTile(index);

		if (tile != null) {

			WallType wall0;
			WallType wall1;
			int wallCount = tile.GetWalls(out wall0, out wall1);
			
			//check with the walls originating from index
			if (wallCount > 0) {
				if (!NonAcuteSequence(type, wall0)) {
					return false;
				}
				if (wallCount > 1) {
					if (!NonAcuteSequence(type, wall1)) {
						return false;
					}
				}
			}
		}

		//check other walls that touch index
		for (byte i = 1; i < 9; i++) {
			VesselTile otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != null) {
				if (otherTile.Contains((WallType)i)) {
					if (Mathf.Abs((byte)type - (byte)i) < 4) {
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
				return false;
			}
		} else if (wall1 > WallType.ZeroByOne) {
			if ((byte)wall0 < (byte)wall1 - 4) {
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
			index -= vc.OriginTileIndex();
			return vc.TileAt(index);
		}
	}
	
	public Vector2 LocalToWorld(Vector2 local)
	{
		return interiorPosition + local;
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
		output.x = (tileI.x >= 0) ? tileI.x / VesselChunk.SIZE : ((tileI.x + 1) / VesselChunk.SIZE) - 1;
		output.y = (tileI.y >= 0) ? tileI.y / VesselChunk.SIZE : ((tileI.y + 1) / VesselChunk.SIZE) - 1;
		return output;
	}

	public bool CutTile(Vec2i tileI, ref WallType type, ref int offset)
	{
		return false;
	}
	
	public static Vec2i ChunkIToTileI(Vec2i chunkI)
	{
		return chunkI * VesselChunk.SIZE;
	}
	
	public static Vector2 TileCenterToLocal(Vec2i tileI)
	{
		return new Vector2(tileI.x + 0.5f, tileI.y + 0.5f);
	}

	public static Vector2 TileToLocal(Vec2i tileI)
	{
		return new Vector2(tileI.x, tileI.y);
	}

	public static Vec2i LocalToTile(Vector2 local)
	{
		return new Vec2i(
			(local.x < 0.0f) ? -(int)(-local.x) - 1 : (int)local.x, 
			(local.y < 0.0f) ? -(int)(-local.y) - 1 : (int)local.y);
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

using UnityEngine;
using UnityEngine.Networking;
using System;
using System.Collections;
using System.Collections.Generic;
using Utility;

public struct VesselChunkData
{
	public Vec2i index;
	public VesselTileData[] data;
	
	public VesselChunkData(Vec2i index)
	{
		this.index = index;
		this.data = new VesselTileData[VesselChunk.DATA_COUNT];
	}
}

public class ChunkMessage : MessageBase
{

}

public class VesselChunk
{
	public static int SIZE_POW = 3;
	public static int SIZE = 1 << SIZE_POW;
	public static int DATA_COUNT = VesselChunk.SIZE * VesselChunk.SIZE;
	public static int OFFSET_MASK = (1 << (SIZE_POW + 1)) - 1;
	public static float WALL_RADIUS = 0.15f;

	public VesselTile[] data;
	public bool Instantiated
	{
		get {
			return instance != null;
		}
	}
	private bool seen;
	public bool Seen
	{
		get {
			return seen;
		}
		set {
			seen = value;
		}
	}
	public GameObject instance;

	private Vec2i index;
	public Vec2i Index
	{
		get{
			return index;
		}
	}
	private bool updateMessageBytes;
	private byte[] messageBytes;
	public byte[] MessageBytes
	{
		get
		{
			if (updateMessageBytes) {
				NetworkWriter nw = new NetworkWriter();

				nw.Write(index.x);
				nw.Write(index.y);

				for (int i = 0; i < VesselChunk.SIZE; i++) {

					for (int j = 0; j < VesselChunk.SIZE; j++) {

						VesselTile tile = TileAt(i, j);
						
						if (tile != null) {

							nw.Write(i);
							nw.Write(j);
							nw.Write((byte)tile.floor0);
							nw.Write((byte)tile.floor1);
							nw.Write((byte)tile.wallMask);
							nw.Write(tile.wallNode);

						}
					}
				}

				messageBytes = nw.ToArray();

				updateMessageBytes = false;
			}

			return messageBytes;
		}
	}
	
	public VesselChunk(Vec2i index)
	{
		this.index = index;
		updateMessageBytes = true;
		data = new VesselTile[VesselChunk.DATA_COUNT];
	}
	public VesselChunk(VesselChunkData data, Vec2i index)
	{
		this.index = index;
		updateMessageBytes = true;
		this.data = new VesselTile[VesselChunk.DATA_COUNT];
	}

	public static Vector2 TileIToOffset(Vec2i tileI)
	{
		return new Vector2(
			(float)tileI.x,
			(float)tileI.y);
	}

	public VesselChunk(byte[] messageBytes)
	{
		updateMessageBytes = true;
		this.data = new VesselTile[VesselChunk.DATA_COUNT];

		NetworkReader nr = new NetworkReader(messageBytes);

		index.x = nr.ReadInt32();
		index.y = nr.ReadInt32();

		while (nr.Position != messageBytes.Length) {
			
			//read in vessel tile
			Vec2i tileI;

			tileI.x = nr.ReadInt32();
			tileI.y = nr.ReadInt32();
			FloorType floor0 = (FloorType)nr.ReadByte();
			FloorType floor1 = (FloorType)nr.ReadByte();
			WallTypeMask wallMask = (WallTypeMask)nr.ReadByte();
			bool wallNode = nr.ReadBoolean();

			VesselTile tile = new VesselTile(wallMask, wallNode, floor0, floor1, (uint)VesselTile.FLAGS.NONE);
			
			SetTile(tileI, tile);
		}
	}
	
	public void SetTile(Vec2i offset, VesselTile val)
	{
		updateMessageBytes = true;
		data[Utility.Utility.MatToVector(offset.x, offset.y, SIZE)] = val;
	}
	
	public VesselTile TileAt(int x, int y)
	{
		try {
			return data[Utility.Utility.MatToVector(x, y, SIZE)];
		} catch (System.Exception ex) {
			throw new ArgumentException();
		}
	}
	
	public VesselTile TileAt(Vec2i index)
	{
		return data[Utility.Utility.MatToVector(index.x, index.y, SIZE)];
	}

	public Vec2i OriginTileIndex()
	{
		return new Vec2i(
			index.x * SIZE,
			index.y * SIZE);
	}

	public void Destroy()
	{
		GameObject.DestroyImmediate(instance);
		instance = null;
	}

	public virtual void Instantiate(VesselChunk t, VesselChunk l, VesselChunk r, VesselChunk b, VesselChunk br, Vector2 position)
	{
		instance = GameObject.Instantiate(GameManager.Instance.chunkPrefab, (Vector3)position, Quaternion.identity) as GameObject;
		instance.name = "Chunk" + Index.ToString();

		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {

				VesselTile tile = TileAt(i,j);

				if (tile != null && tile.wallNode) {

					Vector2 offset = TileIToOffset(new Vec2i(i,j));
					
					GameObject tileGO;
					
					tileGO = GameObject.Instantiate(GameManager.Instance.wallNodePrefab, (Vector3)offset, Quaternion.identity) as GameObject;

					tileGO.name = "Tile" + new Vec2i(i,j).ToString();
					tileGO.transform.SetParent(instance.transform, false);

					WallType wall0;
					WallType wall1;
					int wallCount = tile.GetWalls(out wall0, out wall1);

					if (wallCount > 0) {
						GameObject wall0GO = VesselTile.GetWall(wall0);
						wall0GO.transform.SetParent(tileGO.transform, false);
						if (wallCount > 1) {
							GameObject wall1GO = VesselTile.GetWall(wall1);
							wall1GO.transform.SetParent(tileGO.transform, false);
						}
					}
				}
			}
		}
	}
}


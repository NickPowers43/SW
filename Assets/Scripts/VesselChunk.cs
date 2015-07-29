using UnityEngine;
using UnityEngine.Networking;
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

				for (int i = 0; i < VesselChunk.SIZE * VesselChunk.SIZE; i++) {
					
					VesselTile tile = data[i];
					
					if (tile != null) {
						Vec2i tileI = new Vec2i(i % VesselChunk.SIZE, (i / VesselChunk.SIZE) * VesselChunk.SIZE);
						nw.Write(tileI.x);
						nw.Write(tileI.y);
						nw.Write((byte)tile.floor0);
						nw.Write((byte)tile.floor1);
						nw.Write((byte)tile.wall0T);
						nw.Write((byte)tile.wall1T);
						nw.Write(tile.wallNode);
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
			WallType wall0T = (WallType)nr.ReadByte();
			WallType wall1T = (WallType)nr.ReadByte();
			bool wallNode = nr.ReadBoolean();
			VesselTile tile = new VesselTile(wall0T, wall1T, wallNode, floor0, floor1);
			
			SetTile(tileI, tile);
		}
	}
	
	public void SetTile(Vec2i offset, VesselTile val)
	{
		updateMessageBytes = true;
		this.data[Utility.Utility.MatToVector(offset.x, offset.y, VesselChunk.SIZE)] = val;
	}
	
	public VesselTile TileAt(int x, int y)
	{
		return data[Utility.Utility.MatToVector(x, y, SIZE)];
	}

	public void Destroy()
	{
		GameObject.DestroyImmediate(instance);
		instance = null;
	}

	public virtual void Instantiate(VesselChunk t, VesselChunk l, VesselChunk r, VesselChunk b, VesselChunk br, Vector2 position)
	{
		instance = new GameObject();
		MeshFilter mf = instance.AddComponent<MeshFilter>();
		mf.mesh = GenerateFloorMesh(t, l, r, b, br);

		instance.transform.position = (Vector3)position;
		//return instance;
	}
}


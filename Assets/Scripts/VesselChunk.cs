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

	public void Instantiate(VesselChunk t, VesselChunk l, VesselChunk r, VesselChunk b, VesselChunk br, Vector2 position)
	{
		instance = new GameObject();
		MeshFilter mf = instance.AddComponent<MeshFilter>();
		mf.mesh = GenerateFloorMesh(t, l, r, b, br);

		instance.transform.position = (Vector3)position;
		//return instance;
	}
	
	public Mesh GenerateFloorMesh(VesselChunk t, VesselChunk l, VesselChunk r, VesselChunk b, VesselChunk br)
	{
		List<Vector3> vertices = new List<Vector3>();
		List<Vector2> uv = new List<Vector2>();
		List<int> indices = new List<int>();
		
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				
				VesselTile tile = TileAt(i, j);
				
				if (tile != null) {
					
					Vector3 offset = new Vector3(i, j, 0.0f);
					
					VesselTile tTile = (j == SIZE-1) ? (t != null) ? t.TileAt(i,0) : null : TileAt(i,j+1);
					VesselTile lTile = (i == 0) ? (l != null) ? l.TileAt(SIZE-1,j) : null : TileAt(i-1,j);
					VesselTile rTile = (i == SIZE-1) ? (r != null) ? r.TileAt(0,j) : null : TileAt(i+1,j);
					VesselTile r2Tile = (i >= SIZE-2) ? (r != null) ? r.TileAt(i-SIZE-2,j) : null : TileAt(i+2,j);
					VesselTile bTile = (j == 0) ? (b != null) ? b.TileAt(i,SIZE-1) : null : TileAt(i,j-1);
					VesselTile brTile = null;
					if (i == 0)
						if (j < SIZE-1)
							brTile = (b != null) ? b.TileAt(i+1,SIZE-1) : null;
					else
						brTile = (br != null) ? br.TileAt(0,SIZE-1) : null;
					else
						if (j < SIZE-1)
							brTile = TileAt(i+1,j-1);
					else
						brTile = (r != null) ? r.TileAt(0,j-1) : null;
					
					if (tile.floor0 != FloorType.None || tile.floor1 != FloorType.None) {
						
						if (tile.floor0 == tile.floor1) {
							Sprites.AppendMeshData(Sprites.FloorMeshes[(byte)tile.floor0][(byte)FloorType.None][(byte)WallType.None][0],vertices,uv,indices,offset);
						} else {
							FloorMesh[][] floorCombMeshes = Sprites.FloorMeshes[(byte)tile.floor0][(byte)tile.floor1];
							
							if (tile.Contains((byte)WallType.TwoByOne)) { //this tile contains a TwoByOne
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.TwoByOne][0],vertices,uv,indices,offset);
							} else if (tile.Contains((byte)WallType.OneByTwo)) { //this tile contains a OneByTwo
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByTwo][0],vertices,uv,indices,offset);
							} else if (tile.Contains((byte)WallType.OneByOne)) { //this tile contains a OneByOne
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByOne][0],vertices,uv,indices,offset);
							} 
							else if (lTile != null && lTile.Contains((byte)WallType.TwoByOne)) { //left tile contains a TwoByOne
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.TwoByOne][1],vertices,uv,indices,offset);
							} else if (bTile != null && bTile.Contains((byte)WallType.OneByTwo)) { //bottom tile contains a OneByTwo
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByTwo][1],vertices,uv,indices,offset);
							} else if (brTile != null && brTile.Contains((byte)WallType.OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByTwoFlipped][1],vertices,uv,indices,offset);
							} else if (rTile != null && rTile.Contains((byte)WallType.TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.TwoByOneFlipped][0],vertices,uv,indices,offset);
							} else if (rTile != null && rTile.Contains((byte)WallType.OneByOneFlipped)) { //r tile contains a OneByOneFlipped
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByOneFlipped][0],vertices,uv,indices,offset);
							} else if (rTile != null && rTile.Contains((byte)WallType.OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.OneByTwoFlipped][0],vertices,uv,indices,offset);
							} else if (r2Tile != null && r2Tile.Contains((byte)WallType.TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.TwoByOneFlipped][1],vertices,uv,indices,offset);
							} else {
								//no walls cut this tile
								Sprites.AppendMeshData(floorCombMeshes[(byte)WallType.None][0],vertices,uv,indices,offset);
							}
						}
					}
				}
			}
		}
		
		Mesh output = new Mesh();
		
		output.vertices = vertices.ToArray();
		output.uv = uv.ToArray();
		output.SetIndices(indices.ToArray(), MeshTopology.Triangles, 0);
		
		return output;
	}
}


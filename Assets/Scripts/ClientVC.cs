using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Collections.Generic;
using Utility;

public class ClientVC : VesselChunk
{
	public ClientVC (byte[] messageBytes) :
		base(messageBytes)
	{
	}
	
	public override void Instantiate(VesselChunk t, VesselChunk l, VesselChunk r, VesselChunk b, VesselChunk br, Vector2 position)
	{
		base.Instantiate(t, l, r, b, br, position);

		MeshFilter mf = instance.GetComponent<MeshFilter>();
		mf.mesh = GenerateFloorMesh(t, l, r, b, br);
		
		instance.transform.position = (Vector3)position;
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

					if (tile.floor0 != FloorType.None || tile.floor1 != FloorType.None) {
					
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

						if (tile.floor0 == tile.floor1) {
							Sprites.AppendMeshData(Sprites.FloorMeshes[(byte)tile.floor0][(byte)FloorType.None][(byte)WallType.None][0],vertices,uv,indices,offset);
						} else {
							FloorMesh[][][][] gg = Sprites.FloorMeshes;
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
								Sprites.AppendMeshData(Sprites.FloorMeshes[(byte)tile.floor0][(byte)FloorType.None][(byte)WallType.None][0],vertices,uv,indices,offset);
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


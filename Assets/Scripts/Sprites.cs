using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public struct PosUVPair
{
	public Vector3 position;
	public Vector2 uv;
}
public struct FloorMesh
{
	public int[] indices;
	public PosUVPair[] vertices;

	public FloorMesh(PosUVPair[] vertices, int[] indices)
	{
		this.indices = indices;
		this.vertices = vertices;
	}
}

public class Sprites : MonoBehaviour {

	private static Sprites instance = null;
	public static Sprites Instance
	{
		get{
			return instance;
		}
		set{
			instance = value;
		}
	}

	
	
	public static void AppendMeshData(FloorMesh mesh, List<Vector3> vertices, List<Vector2> uv, List<int> indices, Vector3 offset)
	{
		int startI = vertices.Count;
		for (int i = 0; i < mesh.indices.Length; i++) {
			indices.Add(mesh.indices[i] + startI);
		}

		
		for (int i = 0; i < mesh.vertices.Length; i++) {
			vertices.Add(mesh.vertices[i].position + offset);
			uv.Add(mesh.vertices[i].uv);
		}
	}

	//
	//dim0 - floor0 type
	//dim1 - floor1 type
	//dim2 - wall type
	//dim3 - offset along wall
	private static FloorMesh[][][][] floorMeshes;
	public static FloorMesh[][][][] FloorMeshes
	{
		get{
			return floorMeshes;
		}
		set{
			floorMeshes = value;
		}
	}
	public static void GenerateFloorMeshes()
	{
		floorMeshes = new FloorMesh[FloorUVRects.Length + 1][][][];
		for (int i = 0; i < FloorUVRects.Length + 1; i++) {
			floorMeshes[i] = new FloorMesh[FloorUVRects.Length + 1][][];
		}

		for (int i = 0; i < FloorUVRects.Length + 1; i++) {
			for (int j = 0; j < FloorUVRects.Length + 1; j++) {

				floorMeshes[i][j] = new FloorMesh[9][];

				for (int k = 0; k < 9; k++) {
					floorMeshes[i][j][k] = GenerateMeshes(
						(WallTypeMask)(byte)k, 
						(i == 0) ? FloorUVRects[i] : FloorUVRects[i - 1], 
						i == 0, 
						(j == 0) ? FloorUVRects[j] : FloorUVRects[j - 1], 
						j == 0);
				}
			}
		}
	}

	private static FloorMesh[] GenerateMeshes(WallTypeMask type, Rect rightFloor, bool none0, Rect leftFloor, bool none1)
	{
		if (none0 && none1) {
			return null;
		}

		PosUVPair[] v0 = GenerateBaseVertices(rightFloor);
		PosUVPair[] v1 = GenerateBaseVertices(leftFloor);

		List<int> iTemp = new List<int>();
		List<PosUVPair> vTemp = new List<PosUVPair>();

		FloorMesh[] output;

		switch (type) {
		case WallTypeMask.None:
			//wall type None (simple quad)
			if (!none0) {
				vTemp.Add(v0[0]);
				vTemp.Add(v0[1]);
				vTemp.Add(v0[2]);
				vTemp.Add(v0[3]);
				AddQuad(iTemp);
			}
			return new FloorMesh[1] {new FloorMesh(vTemp.ToArray(), iTemp.ToArray())};
		case WallTypeMask.TwoByOne:
			output = new FloorMesh[2]; //wall type TwoByOne
			if (!none0) {
				vTemp.Add(v0[0]);
				vTemp.Add(v0[7]);
				vTemp.Add(v0[2]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[7]);
				vTemp.Add(v1[3]);
				AddQuad(iTemp);
			}
			output[0] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			vTemp.Clear();
			iTemp.Clear();
			if (!none0) {
				vTemp.Add(v0[0]);
				vTemp.Add(v0[4]);
				vTemp.Add(v0[2]);
				vTemp.Add(v0[3]);
				AddQuad(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[4]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[3]);
				AddTriangle(iTemp);
			}
			output[1] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			return output;
		case WallTypeMask.OneByOne:
			//wall type None (simple quad)
			if (!none0) {
				vTemp.Add(v0[0]);
				vTemp.Add(v0[3]);
				vTemp.Add(v0[2]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[3]);
				AddTriangle(iTemp);
			}
			return new FloorMesh[1] {new FloorMesh(vTemp.ToArray(), iTemp.ToArray())};
		case WallTypeMask.OneByTwo:
			output = new FloorMesh[2]; //wall type OneByTwo
			if (!none0) {
				vTemp.Add(v0[0]);
				vTemp.Add(v0[1]);
				vTemp.Add(v0[2]);
				vTemp.Add(v0[3]);
				AddQuad(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[5]);
				AddTriangle(iTemp);
			}
			output[0] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			vTemp.Clear();
			iTemp.Clear();
			if (!none0) {
				vTemp.Add(v0[6]);
				vTemp.Add(v0[3]);
				vTemp.Add(v0[2]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[6]);
				vTemp.Add(v1[3]);
				AddQuad(iTemp);
			}
			output[1] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			return output;
		case WallTypeMask.OneByTwoFlipped:
			output = new FloorMesh[2]; //wall type OneByTwoFlipped
			if (!none0) {
				vTemp.Add(v0[2]);
				vTemp.Add(v0[5]);
				vTemp.Add(v0[3]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[2]);
				vTemp.Add(v1[5]);
				AddQuad(iTemp);
			}
			output[0] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			vTemp.Clear();
			iTemp.Clear();
			if (!none0) {
				vTemp.Add(v0[6]);
				vTemp.Add(v0[1]);
				vTemp.Add(v0[2]);
				vTemp.Add(v0[3]);
				AddQuad(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[6]);
				AddTriangle(iTemp);
			}
			output[1] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			return output;
		case WallTypeMask.OneByOneFlipped:
			//wall type None (simple quad)
			if (!none0) {
				vTemp.Add(v0[2]);
				vTemp.Add(v0[1]);
				vTemp.Add(v0[3]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[0]);
				vTemp.Add(v1[1]);
				vTemp.Add(v1[2]);
				AddTriangle(iTemp);
			}
			return new FloorMesh[1] {new FloorMesh(vTemp.ToArray(), iTemp.ToArray())};
		case WallTypeMask.TwoByOneFlipped:
			output = new FloorMesh[2]; //wall type OneByTwo
			if (!none0) {
				vTemp.Add(v0[2]);
				vTemp.Add(v0[4]);
				vTemp.Add(v0[3]);
				vTemp.Add(v0[1]);
				AddQuad(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[2]);
				vTemp.Add(v1[0]);
				vTemp.Add(v1[4]);
				AddTriangle(iTemp);
			}
			output[0] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			vTemp.Clear();
			iTemp.Clear();
			if (!none0) {
				vTemp.Add(v0[7]);
				vTemp.Add(v0[1]);
				vTemp.Add(v0[3]);
				AddTriangle(iTemp);
			}
			if (!none1) {
				vTemp.Add(v1[2]);
				vTemp.Add(v1[0]);
				vTemp.Add(v1[7]);
				vTemp.Add(v1[1]);
				AddQuad(iTemp);
			}
			output[1] = new FloorMesh(vTemp.ToArray(), iTemp.ToArray());
			return output;
		default:
			return null;
		}
	}

	private static void AddTriangle(List<int> iTemp)
	{
		int start = iTemp.Count;
		iTemp.Add(start + 0);
		iTemp.Add(start + 1);
		iTemp.Add(start + 2);
	}

	private static void AddQuad(List<int> iTemp)
	{
		int start = iTemp.Count;
		iTemp.Add(start + 0);
		iTemp.Add(start + 1);
		iTemp.Add(start + 2);
		iTemp.Add(start + 1);
		iTemp.Add(start + 3);
		iTemp.Add(start + 2);
	}

	private static PosUVPair[] GenerateBaseVertices(Rect a)
	{
		PosUVPair[] output = new PosUVPair[8];

		PosUVPair temp;
		temp.position = Vector3.zero;
		temp.uv = new Vector2(a.x, a.y);
		output[0] = temp;
		
		temp.position = Vector3.up;
		temp.uv = new Vector2(a.x, a.y + a.height);
		output[1] = temp;
		
		temp.position = Vector3.right;
		temp.uv = new Vector2(a.x + a.width, a.y);
		output[2] = temp;
		
		temp.position = new Vector3(1.0f, 1.0f);
		temp.uv = new Vector2(a.x + a.width, a.y + a.height);
		output[3] = temp;

		
		temp.position = (Vector3.up * 0.5f);
		temp.uv = new Vector2(a.x, a.y + (a.height * 0.5f));
		output[4] = temp;

		temp.position = Vector3.up + (Vector3.right * 0.5f);
		temp.uv = new Vector2(a.x + (a.width * 0.5f), a.y + a.height);
		output[5] = temp;
		
		temp.position = (Vector3.right * 0.5f);
		temp.uv = new Vector2(a.x + (a.width * 0.5f), a.y);
		output[6] = temp;
		
		temp.position = (Vector3.up * 0.5f) + Vector3.right;
		temp.uv = new Vector2(a.x + a.width, a.y + (a.height * 0.5f));
		output[7] = temp;

		return output;
	}

	private static Rect[] floorUVRects;
	public static Rect[] FloorUVRects
	{
		get{
			return floorUVRects;
		}
		set{
			floorUVRects = value;
		}
	}


	public static void Initialize(Sprite[] floors)
	{
		floorUVRects = new Rect[floors.Length];
		Rect[] dflkjs = floorUVRects;

		for (int i = 0; i < floors.Length; i++) {
			floorUVRects[i] = GetRect(floors[i]);
		}

		GenerateFloorMeshes();
	}
	public static Rect GetRect(Sprite a)
	{
		return new Rect(
			a.rect.xMin / a.texture.width, 
			a.rect.yMin / a.texture.height, 
			(a.rect.xMax - a.rect.xMin) / a.texture.width, 
			(a.rect.yMax - a.rect.yMin) / a.texture.height);
	}
	public static void GenerateFloorMesh(Mesh mesh, VesselChunk chunk)
	{

	}

	public Sprite wall_small;
	public Sprite wall_medium;
	public Sprite wall_large;
	
	public Sprite wall_node;

	public Sprite[] floors;

	void Start () {
	
		if (Instance == null) {
			Instance = this;
			Initialize(this.floors);
		} else {
			DestroyImmediate(this);
		}
	}
}

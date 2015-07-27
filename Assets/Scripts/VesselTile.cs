using UnityEngine;
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

public struct VesselTileData
{
	public WallType wall0;
	public WallType wall1;
}

public class VesselTile
{
	public static float TWO_BY_ONE_ROT = Mathf.Rad2Deg * Mathf.Atan2(1, 2);
	public static float ONE_BY_TWO_ROT = Mathf.Rad2Deg * Mathf.Atan2(2, 1);
	
	public static Quaternion ONE_BY_ZERO_QUAT = Quaternion.identity;
	public static Quaternion TWO_BY_ONE_QUAT = Quaternion.AngleAxis(TWO_BY_ONE_ROT, Vector3.back);
	public static Quaternion ONE_BY_ONE_QUAT = Quaternion.AngleAxis(45.0f, Vector3.back);
	public static Quaternion ONE_BY_TWO_QUAT = Quaternion.AngleAxis(ONE_BY_TWO_ROT, Vector3.back);
	public static Quaternion ZERO_BY_ONE_QUAT = Quaternion.AngleAxis(90.0f, Vector3.back);
	public static Quaternion TWO_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(TWO_BY_ONE_ROT + 90.0f, Vector3.back);
	public static Quaternion ONE_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(45.0f + 90.0f, Vector3.back);
	public static Quaternion ONE_BY_TWO_FLIPPED_QUAT = Quaternion.AngleAxis(ONE_BY_TWO_ROT + 90.0f, Vector3.back);
	
	public SpriteRenderer wallNode;
	public WallType wall0T;
	public WallType wall1T;
	public SpriteRenderer wall0;
	public SpriteRenderer wall1;
	public FloorType floor0;
	public FloorType floor1;
	
	private static SpriteRenderer GetWall(WallType type)
	{
//		switch (type) {
//		case WallType.OneByZero:
//			return GameObject.Instantiate(Editor.WallSmall, Vector3.zero, Quaternion.identity) as SpriteRenderer;
//		case WallType.TwoByOne:
//			return GameObject.Instantiate(Editor.WallLarge, Vector3.zero, TWO_BY_ONE_QUAT) as SpriteRenderer;
//		case WallType.OneByOne:
//			return GameObject.Instantiate(Editor.WallMedium, Vector3.zero, ONE_BY_ONE_QUAT) as SpriteRenderer;
//		case WallType.OneByTwo:
//			return GameObject.Instantiate(Editor.WallLarge, Vector3.zero, ONE_BY_TWO_QUAT) as SpriteRenderer;
//		case WallType.ZeroByOne:
//			return GameObject.Instantiate(Editor.WallSmall, Vector3.zero, ZERO_BY_ONE_QUAT) as SpriteRenderer;
//		case WallType.OneByTwoFlipped:
//			return GameObject.Instantiate(Editor.WallSmall, Vector3.zero, ONE_BY_TWO_FLIPPED_QUAT) as SpriteRenderer;
//		case WallType.OneByOneFlipped:
//			return GameObject.Instantiate(Editor.WallSmall, Vector3.zero, ONE_BY_ONE_FLIPPED_QUAT) as SpriteRenderer;
//		case WallType.TwoByOneFlipped:
//			return GameObject.Instantiate(Editor.WallSmall, Vector3.zero, TWO_BY_ONE_FLIPPED_QUAT) as SpriteRenderer;
//		default:
			return null;
//			break;
//		}
	}
	
	public VesselTile(WallType wall0, WallType wall1, bool wallNode, FloorType floor0, FloorType floor1)
	{
		this.wall0T = wall0;
		this.wall1T = wall1;
		//this.wall0 = GetWall (wall0);
		//this.wall1 = GetWall (wall1);
		//this.wallNode = (wallNode) ? GameObject.Instantiate(Editor.WallNode, Vector3.zero, Quaternion.identity) as SpriteRenderer : null;
		this.floor0 = floor0;
		this.floor1 = floor1;
	}
	
	public void SetPosition(Vector3 position)
	{
		wallNode.transform.position = position;
		wall0.transform.position = position;
		wall1.transform.position = position;
	}
	
	public bool Contains(byte wall)
	{
		return (WallType)wall == wall0T || (WallType)wall == wall1T;
	}
}


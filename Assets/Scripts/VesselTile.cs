using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Utility;

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
	public static Quaternion TWO_BY_ONE_QUAT = Quaternion.AngleAxis(TWO_BY_ONE_ROT, Vector3.forward);
	public static Quaternion ONE_BY_ONE_QUAT = Quaternion.AngleAxis(45.0f, Vector3.forward);
	public static Quaternion ONE_BY_TWO_QUAT = Quaternion.AngleAxis(ONE_BY_TWO_ROT, Vector3.forward);
	public static Quaternion ZERO_BY_ONE_QUAT = Quaternion.AngleAxis(90.0f, Vector3.forward);
	public static Quaternion TWO_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(TWO_BY_ONE_ROT + 90.0f, Vector3.forward);
	public static Quaternion ONE_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(45.0f + 90.0f, Vector3.back);
	public static Quaternion ONE_BY_TWO_FLIPPED_QUAT = Quaternion.AngleAxis(ONE_BY_TWO_ROT + 90.0f, Vector3.forward);
	
	public bool wallNode;
	public WallType wall0T;
	public WallType wall1T;
	public FloorType floor0;
	public FloorType floor1;
	
	public static GameObject GetWall(WallType type)
	{
		switch (type) {
		case WallType.OneByZero:
			return GameObject.Instantiate(GameManager.Instance.wallSmallPrefab, Vector3.zero, Quaternion.identity) as GameObject;
		case WallType.TwoByOne:
			return GameObject.Instantiate(GameManager.Instance.wallLargePrefab, Vector3.zero, TWO_BY_ONE_QUAT) as GameObject;
		case WallType.OneByOne:
			return GameObject.Instantiate(GameManager.Instance.wallMediumPrefab, Vector3.zero, ONE_BY_ONE_QUAT) as GameObject;
		case WallType.OneByTwo:
			return GameObject.Instantiate(GameManager.Instance.wallLargePrefab, Vector3.zero, ONE_BY_TWO_QUAT) as GameObject;
		case WallType.ZeroByOne:
			return GameObject.Instantiate(GameManager.Instance.wallSmallPrefab, Vector3.zero, ZERO_BY_ONE_QUAT) as GameObject;
		case WallType.OneByTwoFlipped:
			return GameObject.Instantiate(GameManager.Instance.wallSmallPrefab, Vector3.zero, ONE_BY_TWO_FLIPPED_QUAT) as GameObject;
		case WallType.OneByOneFlipped:
			return GameObject.Instantiate(GameManager.Instance.wallSmallPrefab, Vector3.zero, ONE_BY_ONE_FLIPPED_QUAT) as GameObject;
		case WallType.TwoByOneFlipped:
			return GameObject.Instantiate(GameManager.Instance.wallSmallPrefab, Vector3.zero, TWO_BY_ONE_FLIPPED_QUAT) as GameObject;
		default:
			return null;
			break;
		}
	}
	
	public VesselTile(WallType wall0, WallType wall1, bool wallNode, FloorType floor0, FloorType floor1)
	{
		this.wall0T = wall0;
		this.wall1T = wall1;
		this.wallNode = wallNode;
		this.floor0 = floor0;
		this.floor1 = floor1;
	}
	
	public bool Contains(byte wall)
	{
		return (WallType)wall == wall0T || (WallType)wall == wall1T;
	}
}


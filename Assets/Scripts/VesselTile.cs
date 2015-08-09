using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Utility;

public struct VesselTileData
{
	public WallTypeMask wall0;
	public WallTypeMask wall1;
}

public class VesselTile
{
	public enum FLAGS : uint
	{
		NONE = 0,
		HATCH0 = 1 << 0,
		HATCH1 = 1 << 1
	}

	public static float TWO_BY_ONE_ROT = Mathf.Rad2Deg * Mathf.Atan2(1, 2);
	public static float ONE_BY_TWO_ROT = Mathf.Rad2Deg * Mathf.Atan2(2, 1);
	
	public static Quaternion ONE_BY_ZERO_QUAT = Quaternion.identity;
	public static Quaternion TWO_BY_ONE_QUAT = Quaternion.AngleAxis(TWO_BY_ONE_ROT, Vector3.forward);
	public static Quaternion ONE_BY_ONE_QUAT = Quaternion.AngleAxis(45.0f, Vector3.forward);
	public static Quaternion ONE_BY_TWO_QUAT = Quaternion.AngleAxis(ONE_BY_TWO_ROT, Vector3.forward);
	public static Quaternion ZERO_BY_ONE_QUAT = Quaternion.AngleAxis(90.0f, Vector3.forward);
	public static Quaternion ONE_BY_TWO_FLIPPED_QUAT = Quaternion.AngleAxis(180.0f - ONE_BY_TWO_ROT, Vector3.forward);
	public static Quaternion ONE_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(180.0f - 45.0f, Vector3.forward);
	public static Quaternion TWO_BY_ONE_FLIPPED_QUAT = Quaternion.AngleAxis(180.0f - TWO_BY_ONE_ROT, Vector3.forward);
	
	public bool wallNode;
	public byte wallMask;
	public uint flags;
	public FloorType floor0;
	public FloorType floor1;
	public BlockType blockT;
	
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
			return GameObject.Instantiate(GameManager.Instance.wallLargePrefab, Vector3.zero, ONE_BY_TWO_FLIPPED_QUAT) as GameObject;
		case WallType.OneByOneFlipped:
			return GameObject.Instantiate(GameManager.Instance.wallMediumPrefab, Vector3.zero, ONE_BY_ONE_FLIPPED_QUAT) as GameObject;
		case WallType.TwoByOneFlipped:
			return GameObject.Instantiate(GameManager.Instance.wallLargePrefab, Vector3.zero, TWO_BY_ONE_FLIPPED_QUAT) as GameObject;
		default:
			return null;
			break;
		}
	}
	
	public VesselTile()
	{
		this.flags = (uint)FLAGS.NONE;
		this.wallMask = 0;
		this.wallNode = false;
		this.floor0 = FloorType.None;
		this.floor1 = FloorType.None;
	}
	
	public VesselTile(WallTypeMask wallMask, bool wallNode, FloorType floor0, FloorType floor1, uint flags)
	{
		this.flags = flags;
		this.wallMask = (byte)wallMask;
		this.wallNode = wallNode;
		this.floor0 = floor0;
		this.floor1 = floor1;
	}
	
	public int WallCount()
	{
		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
			}
		}

		return count;
	}

	public int GetWalls(out WallType wall0, out WallType wall1)
	{
		wall0 = WallType.None;
		wall1 = WallType.None;
		bool firstSet = false;

		int count = 0;
		
		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
				if (!firstSet) {
					wall0 = (WallType)(i + 1);
					firstSet = true;
				} else {
					wall1 = (WallType)(i + 1);
				}
			}
		}
		
		return count;
	}
	
	public bool Contains(WallType wall)
	{
		return (wallMask & (1 << ((byte)wall - 1))) > 0;
	}
	
	public bool Contains(WallTypeMask wall)
	{
		return (wallMask & (byte)wall) > 0;
	}
}


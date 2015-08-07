using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class StartingVessel : ServerVessel
{
	public StartingVessel () :
		base()
	{
		Initialize();

		Vessels.Add(this);
	}

	public override void Update()
	{
		base.Update();
	}

	private void Initialize()
	{
		VesselTile tempTile;

		Vec2i tempI = new Vec2i(VesselChunk.SIZE >> 1, VesselChunk.SIZE >> 1);
		//tempTile = new VesselTile(WallType.OneByZero, WallType.ZeroByOne, true, FloorType.Basic, FloorType.None);
		//SetTile(tempI, tempTile);
		BuildWall(tempI, 10, WallType.ZeroByOne);
		BuildWall(tempI, 10, WallType.OneByZero);
		BuildWall(tempI, 10, WallType.OneByOne);

		tempI += new Vec2i(3, 0);
		BuildWall(tempI, 10, WallType.ZeroByOne);

		tempI += new Vec2i(3, 0);
		BuildWall(tempI, 10, WallType.ZeroByOne);
		BuildWall(tempI, 10, WallType.TwoByOne);
		BuildWall(tempI, 10, WallType.TwoByOneFlipped);

		tempI += new Vec2i(4, 0);
		BuildWall(tempI, 10, WallType.OneByOneFlipped);

		//tempI += new Vec2i(3, -3);
		//tempTile = new VesselTile(WallType.None, WallType.None, true, FloorType.None, FloorType.None);
		//SetTile(tempI, tempTile);
		//BuildWall(tempI, 3, WallType.OneByZero);

		//tempI += new Vec2i(0, -1);
		//tempTile = new VesselTile(WallType.ZeroByOne, WallType.None, true, FloorType.None, FloorType.None);
		//SetTile(tempI, tempTile);
		//BuildWall(tempI, 3, WallType.OneByZero);
	}

	public static StartingVessel GetStartingVessel()
	{
		StartingVessel output;
		
		//find start vessel
		if (Vessels.Count == 0) {
			//create one
			
			return new StartingVessel();
		} else {
			return Vessels[0];
		}
	}

	private static List<StartingVessel> Vessels = new List<StartingVessel>(64);
}


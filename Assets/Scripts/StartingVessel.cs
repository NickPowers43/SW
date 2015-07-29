using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class StartingVessel : ServerVessel
{
	public StartingVessel ()
	{
		Initialize();

		ActiveVessels.Add(this);
	}

	public override void Update()
	{
		base.Update();
	}

	private void Initialize()
	{
		Vec2i tempI = new Vec2i(VesselChunk.SIZE >> 1, VesselChunk.SIZE >> 1);
		
		VesselTile tempTile;
		
		tempTile = new VesselTile(WallType.OneByZero, WallType.ZeroByOne, true, FloorType.Basic, FloorType.None);
		SetTile(tempI, tempTile);
		tempI += new Vec2i(0, 1);
		tempTile = new VesselTile(WallType.OneByZero, WallType.None, true, FloorType.None, FloorType.None);
		SetTile(tempI, tempTile);
		tempI += new Vec2i(1, 0);
		tempTile = new VesselTile(WallType.None, WallType.None, true, FloorType.None, FloorType.None);
		SetTile(tempI, tempTile);
		tempI += new Vec2i(0, -1);
		tempTile = new VesselTile(WallType.ZeroByOne, WallType.None, true, FloorType.None, FloorType.None);
		SetTile(tempI, tempTile);
	}

	public static StartingVessel GetStartingVessel()
	{
		StartingVessel output;
		
		//find start vessel
		if (ActiveVessels.Count == 0) {
			//create one
			
			return new StartingVessel();
		} else {
			return ActiveVessels[0];
		}
	}

	private static List<StartingVessel> ActiveVessels = new List<StartingVessel>(64);
}


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
		if (true) {
			//create one
			
			output = new StartingVessel();
		}
		
		return output;
	}

	private static List<StartingVessel> activeVessels = new List<StartingVessel>(64);
	public static List<StartingVessel> ActiveVessels
	{
		get{
			return activeVessels;
		}
		set{
			activeVessels = value;
		}
	}
}


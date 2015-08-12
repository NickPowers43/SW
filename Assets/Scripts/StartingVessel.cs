using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class StartingVessel : ServerVessel
{
	private Vec2i spawnerLocation;

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
		BuildFoundation(new Vec2i(0,0), new Vec2i(8, 16));
		//tip
		BuildWall(new Vec2i(4,13), 1, WallType.OneByZero);
		//outer left
		//
		BuildWall(new Vec2i(1,1), 1, WallType.ZeroByOne);
		BuildWall(new Vec2i(1,2), 1, WallType.OneByTwo);
		BuildWall(new Vec2i(2,4), 5, WallType.ZeroByOne);
		BuildWall(new Vec2i(2,9), 1, WallType.OneByTwo);
		BuildWall(new Vec2i(3,11), 1, WallType.ZeroByOne);
		BuildWall(new Vec2i(3,12), 1, WallType.OneByOne);
		//outer right
		//
		BuildWall(new Vec2i(8,1), 1, WallType.ZeroByOne);
		BuildWall(new Vec2i(8,2), 1, WallType.OneByTwoFlipped);
		BuildWall(new Vec2i(7,4), 5, WallType.ZeroByOne);
		BuildWall(new Vec2i(7,9), 1, WallType.OneByTwoFlipped);
		BuildWall(new Vec2i(6,11), 1, WallType.ZeroByOne);
		BuildWall(new Vec2i(6,12), 1, WallType.OneByOneFlipped);

		//inner vertical walls
		BuildWall(new Vec2i(4,4), 7, WallType.ZeroByOne);
		BuildWall(new Vec2i(5,4), 7, WallType.ZeroByOne);
		//left horizontal walls
		BuildWall(new Vec2i(3,11), 1, WallType.OneByZero);
		BuildWall(new Vec2i(2,7), 2, WallType.OneByZero);
		BuildWall(new Vec2i(2,4), 2, WallType.OneByZero);
		//right horizontal walls
		BuildWall(new Vec2i(5,11), 1, WallType.OneByZero);
		BuildWall(new Vec2i(5,7), 2, WallType.OneByZero);
		BuildWall(new Vec2i(5,4), 2, WallType.OneByZero);

		//place spawner for noobs
		PlaceBlock(BlockType.Spawner, new Vec2i(4, 2));
	}

	public override bool PlaceBlock(BlockType type, Vec2i location)
	{
		if (type == BlockType.Spawner) {
			spawnerLocation = location;
		}

		return base.PlaceBlock(type, location);
	}

	public void AddPlayer(Character2D player)
	{
		base.AddPlayer(player, TileCenterToLocal(spawnerLocation));
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





using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using Utility;

public class ServerPlayer : Player {

	public ServerVessel currentVessel;
	public int hostId;
	public int connectionId;
	public PlayerInfo pi;

	void Start () {
		BaseStart();
	}

	void Update () {
		
	}

	public void ReadInputsMessage(NetworkReader nr)
	{
		Vector2 movement = nr.ReadVector2();
		Vector2 facing = nr.ReadVector2();
		Move(movement, facing);
	}
}


using UnityEngine;

public class PlayerInfo
{
	public string username;
	public string password;

	public Vector2 lastPosition;
	public Vessel lastVessel;

	public PlayerInfo ()
	{
		this.lastVessel = null;
		this.lastPosition = Vector2.zero;
		this.username = null;
		this.password = null;
	}
}


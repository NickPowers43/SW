using System;
using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Utility;

public class MyMsgType {
	public static short Chunk = MsgType.Highest + 1;
};

public class Character2D : NetworkBehaviour
{
	public float maxSpeed = 5.5f;
	
	//private Animator m_Anim;            // Reference to the player's animator component.
	private Rigidbody2D m_Rigidbody2D;
	private float baseRot;
	private PlayerInfo pi;
	private NetworkClient nc;

	private Vec2i chunkI;
	public Vec2i ChunkI
	{
		get {
			return chunkI;
		}
		set {
			chunkI = value;
		}
	}
	
	public void Start()
	{
		m_Rigidbody2D = GetComponent<Rigidbody2D>();
		baseRot = transform.rotation.eulerAngles.z;

		if (isLocalPlayer) {

			CmdRegister("Nick", "acrylic43");
		}
	}
	
	[Command(channel=1)]
	public void CmdHandleShiftClick(Vector2 location)
	{   
		MainCamera.Instance.transform.position = (Vector3)location;
	}

	[Command(channel=1)]
	public void CmdHandleInputs(float hAxis, float vAxis, Vector2 cursorPosition)
	{   
		Vector2 facing = cursorPosition - (Vector2)transform.position;
		Vector2 movement = new Vector2(hAxis, vAxis);
		Move(movement, facing);
	}

	[Command(channel=0)]
	public void CmdLogin(string username, string password)
	{
		foreach (PlayerInfo item in playerAccounts) {
			
			if (item.username == username) {
				if (item.password == password) {
					pi = item;
					
					//account is found (pi)
					//start this player where they left off
					pi.lastVessel.AddPlayer(this, pi.lastPosition);
					
					RpcLoginSuccessfull(username);
					Debug.Log ("player \"" + username + "\" registration successful"); 
					return;
				} else {
					RpcLoginFailPassword();
					Debug.Log ("player \"" + username + "\" gave incorrect password"); 
				}
			}
			
		}

		RpcUsernameDoesNotExist(username);
		Debug.Log ("Rejecting player \"" + username + "\" login: username does not exist");
	}
	
	[Command(channel=0)]
	public void CmdRegister(string username, string password)
	{
		foreach (PlayerInfo item in playerAccounts) {

			if (item.username == username) {
				RpcUsernameTaken(username);
				Debug.Log ("Rejecting player \"" + username + "\" registration: username taken"); 
				return;
			}

		}

		//username is unique
		pi = new PlayerInfo();
		pi.username = username;
		pi.password = password;
		PlayerAccounts.Add(pi);
		
		//start this player off from the beginning
		Vessel startVessel = Vessel.GetStartingVessel();
		startVessel.AddPlayer(this);
		
		RpcLoginSuccessfull(username);
		Debug.Log ("player \"" + username + "\" login successful"); 
	}
	
	[ClientRpc(channel=0)]
	public void RpcCreateChunk(byte[] message)
	{
		Debug.Log("Receiving chunk");
		VesselChunk vc = new VesselChunk(message);

	}
	
	[ClientRpc]
	void RpcUsernameDoesNotExist(string username)
	{
		Debug.Log(username + " does not exist");
	}

	[ClientRpc]
	void RpcUsernameTaken(string username)
	{
		Debug.Log(username + " is taken");
	}
	
	[ClientRpc]
	void RpcLoginSuccessfull(string username)
	{
		Debug.Log(username + " logged in");
	}
	
	[ClientRpc]
	void RpcLoginFailPassword()
	{
		Debug.Log("incorrect password");
	}

	[ClientCallback]
	private void Update()
	{
		if (isLocalPlayer) {

			Vector2 cursorPosition = (Vector2)MainCamera.Instance.camera.ScreenToWorldPoint(Input.mousePosition);

			CmdHandleInputs(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"), cursorPosition);

			if (Input.GetKey(KeyCode.LeftShift) && Input.GetMouseButtonDown(0)) {
				CmdHandleShiftClick(cursorPosition);
			}

			MainCamera.Instance.SetTarget(transform.position);
		}
	}
	
	public void Move(Vector2 move, Vector2 facing)
	{
		Quaternion q = new Quaternion();
		float angle = (Mathf.Rad2Deg * Mathf.Atan2(facing.y, facing.x));
		q.eulerAngles = new Vector3(0.0f, 0.0f, angle + baseRot);
		transform.rotation = q;
		
		// Move the character
		m_Rigidbody2D.velocity = move * maxSpeed;
		
	}

	private static List<PlayerInfo> playerAccounts = new List<PlayerInfo>(512);
	public static List<PlayerInfo> PlayerAccounts
	{
		get{
			return playerAccounts;
		}
		set{
			playerAccounts = value;
		}
	}
}

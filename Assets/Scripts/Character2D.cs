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
	public static float OBSERVER_UPDATE_INTERVAL = 2.0f;
	public static float NETWORK_VIS_RANGE_SQR = 15.0f * 15.0f;

	public float maxSpeed = 5.5f;
	public Vessel currentVessel;

	private Rigidbody2D rb;
	private float baseRot;
	private PlayerInfo pi;

	private bool hidden = false;
	public NetworkIdentity networkIdentity;
	private bool initialObserverRebuild = true;

	private SimpleTimer observerUpdateTimer;

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
		observerUpdateTimer = new SimpleTimer(OBSERVER_UPDATE_INTERVAL);

		networkIdentity = GetComponent<NetworkIdentity>();
		rb = GetComponent<Rigidbody2D>();
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
	public void CmdBuildWall(int x, int y, int count, int wallType)
	{
		ServerVessel cv = (ServerVessel)currentVessel;


	}

	[Command(channel=0)]
	public void CmdFillAt(Vector2 local)
	{
		Debug.Log("Filling At: " + local.ToString());
		ServerVessel cv = (ServerVessel)currentVessel;
		
		cv.SetCompartmentFloor(FloorType.Basic, cv.CompartmentAt(local));
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

			if (false && item.username == username) {
				RpcUsernameTaken(username);
				Debug.Log ("Rejecting player \"" + username + "\" registration: username taken"); 
				return;
			}

		}

		//username is unique
		pi = new PlayerInfo();
		pi.username = username + DateTime.Today.ToString();
		pi.password = password;
		PlayerAccounts.Add(pi);
		
		//start this player off from the beginning
		StartingVessel startVessel = StartingVessel.GetStartingVessel();
		startVessel.AddPlayer(this);
		
		RpcLoginSuccessfull(username);
		Debug.Log ("player \"" + username + "\" login successful"); 
	}
	
	[ClientRpc(channel=0)]
	public void RpcSyncVessel(uint vesselIndex, byte[] message)
	{
		Debug.Log("Syncing Vessel: " + vesselIndex);

		ClientVessel cv;
		if (!ClientVessel.Vessels.TryGetValue(vesselIndex, out cv)) {
			cv = new ClientVessel(vesselIndex, message);
		}

		currentVessel = cv;
	}
	
	[ClientRpc(channel=0)]
	public void RpcSetChunk(uint vesselIndex, byte[] message)
	{
		ClientVessel cv;
		if (!ClientVessel.Vessels.TryGetValue(vesselIndex, out cv)) {
			Debug.Log ("Missing Vessel");
			return;
		}

		Debug.Log("Receiving chunk");
		ClientVC vc = new ClientVC(message);

		cv.SetChunk(vc);
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

	private void Update()
	{
		if (isServer){
			
			if (observerUpdateTimer.Update()) {
				networkIdentity.RebuildObservers(initialObserverRebuild);
				initialObserverRebuild = false;
				observerUpdateTimer.Set(OBSERVER_UPDATE_INTERVAL);
			}
			
		} else {

			if (isLocalPlayer) {

				Vector2 cursorPosition = (Vector2)MainCamera.Instance.camera.ScreenToWorldPoint(Input.mousePosition);
				
				CmdHandleInputs(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"), cursorPosition);
				
				if (Input.GetKey(KeyCode.LeftShift) && Input.GetMouseButtonDown(0)) {
					CmdHandleShiftClick(cursorPosition);
				}

				if (Input.GetMouseButtonDown(0)) {
					Vector2 mouseWorld = (Vector2)MainCamera.Instance.camera.ScreenToWorldPoint(Input.mousePosition);
					Debug.Log("Sending FillAt command for location: " + mouseWorld.ToString());
					CmdFillAt(mouseWorld - currentVessel.LocalToWorld(Vector2.zero));
				}
				
				MainCamera.Instance.SetTarget(transform.position);

			}
		}
	}
	
	public void Move(Vector2 move, Vector2 facing)
	{
		Quaternion q = new Quaternion();
		float angle = (Mathf.Rad2Deg * Mathf.Atan2(facing.y, facing.x));
		q.eulerAngles = new Vector3(0.0f, 0.0f, angle + baseRot);
		transform.rotation = q;
		
		// Move the character
		rb.velocity = move * maxSpeed;
		
	}

	public override bool OnCheckObserver (NetworkConnection newObserver)
	{
		for (int i = 0; i < newObserver.playerControllers.Count; i++) {

			Vector2 diff = (Vector2)newObserver.playerControllers[i].gameObject.transform.position - (Vector2)transform.position;
			
			if (diff.sqrMagnitude < NETWORK_VIS_RANGE_SQR) {

				return true;
			}
			
		}

		return false;
	}

	public override bool OnRebuildObservers (HashSet<NetworkConnection> observers, bool initial)
	{
		bool output = false;

		if (networkIdentity != null) {
			if (networkIdentity.connectionToClient != null)
			{
				observers.Add (networkIdentity.connectionToClient);
				output |= true;
			}
		}

		if (!hidden) {
			
			if (currentVessel != null) {
				
				ServerVessel cv = (ServerVessel)currentVessel;
				
				for (int i = 0; i < cv.netIdentities.Count; i++) {

					try {
						if (cv.netIdentities[i].connectionToClient != null) {
							
							Vector2 diff = (Vector2)cv.netIdentities[i].transform.position - (Vector2)transform.position;
							
							if (diff.sqrMagnitude < NETWORK_VIS_RANGE_SQR) {

								observers.Add (cv.netIdentities[i].connectionToClient);
								output |= true;
								
							}
						}
					} catch (Exception ex) {
						cv.netIdentities.RemoveAt(i);
					}
				}
			}
		}

		return output;
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

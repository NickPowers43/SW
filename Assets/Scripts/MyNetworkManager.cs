using UnityEngine;
using UnityEngine.Networking;
using System.Collections;

public class MyNetworkManager : NetworkManager {

//	void Update () {
//	
//		if (Network.isServer) {
//
//			for (int i = 0; i < Vessel.ActiveVessels.Count; i++) {
//
//				Vessel.ActiveVessels[i].Update();
//			}
//
//		}
//	}

	//client-side
	public virtual void OnClientConnect(NetworkConnection conn)
	{
		Debug.Log("OnClientConnect");
		//connection is successful
		ClientScene.Ready(conn);
		ClientScene.AddPlayer(conn, 0);
	}
//
//	//server-side
//	public virtual void OnServerConnect(NetworkConnection conn)
//	{
//		//client has connected
//		
//		base.OnServerConnect(conn);
//	}
//	public virtual void OnServerReady(NetworkConnection conn)
//	{
//		//client is ready
//
//		base.OnServerReady(conn);
//	}
	public virtual void OnServerAddPlayer(NetworkConnection conn, short playerControllerId)
	{
		Debug.Log("Adding Player");
		// called when a new player is added for a client
		var player = (GameObject)GameObject.Instantiate(playerPrefab, Vector3.zero, Quaternion.identity);

		if (!NetworkServer.AddPlayerForConnection(conn, player, playerControllerId)) {
			Debug.Log("Failed to add player for connetion");
		}
	}
//	public virtual void OnServerRemovePlayer(NetworkConnection conn, short playerControllerId)
//	{
//		// called when a player is removed for a client
//		UPlayer player;
//		if (conn.GetPlayer(playerControllerId, out player))
//		{
//			if (player.NetworkIdentity != null && player.NetworkIdentity.gameObject != null)
//				NetworkServer.Destroy(player.NetworkIdentity.gameObject);
//		}
//	}
}

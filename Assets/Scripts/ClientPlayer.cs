using UnityEngine;
using UnityEngine.Networking;
using System.Collections;

public class ClientPlayer : Player {

	public bool myPlayer;
	public ClientVessel currentVessel;

	void Start () {
		myPlayer = false;
	}

	void Update () {

		if (myPlayer) {
			MainCamera.Instance.SetTarget(transform.position);

			Move(new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical")),
			     MainCamera.CursorPosition);
		}
	}

	public void WriteInputMessage(NetworkWriter nw)
	{
		nw.Write((ushort)ClientMessageType.Inputs);
		nw.Write(new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical")));
	    nw.Write(MainCamera.CursorPosition);
	}
}

using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class ClientVessel : Vessel
{
	public ClientVessel (uint index, byte[] messageBytes) :
		base(index)
	{
		NetworkReader nr = new NetworkReader(messageBytes);
		
		this.interiorPosition = nr.ReadVector2();
		
		Vessels.Add(index, this);
	}

	public void SetChunk(ClientVC chunk)
	{
		VesselChunk current = chunks.TryGet(chunk.Index);

		if (current != null) {
			Debug.Log ("Need to destroy existing chunk");
		}

		Debug.Log ("Setting chunk (" + chunk.Index.x + ", " + chunk.Index.y + ")");
		chunks.Set(chunk.Index, chunk);

		foreach (var c in chunks.WithinRange(chunk.Index, 1)) {

			if (c != null) {

				if (c.Instantiated) {
					c.Destroy();
				}
				
				InstantiateChunk(c);
			}

		}
	}
	
	public static Dictionary<uint, ClientVessel> Vessels = new Dictionary<uint, ClientVessel>();
}


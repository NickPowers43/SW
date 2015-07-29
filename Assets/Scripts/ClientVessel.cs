using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class ClientVessel : Vessel
{
	public ClientVessel (uint index) :
		base(index)
	{
		Vessels.Add(index, this);
	}
	public ClientVessel (uint index, byte[] messageBytes) :
		base(index)
	{
		ReadMessage(messageBytes);
		
		Vessels.Add(index, this);
	}
	
	public void ReadMessage(byte[] messageBytes)
	{
		NetworkReader nr = new NetworkReader(messageBytes);
		
		this.interiorPosition = nr.ReadVector2();
	}

	public void SetChunk(VesselChunk chunk)
	{

	}
	
	public static Dictionary<uint, ClientVessel> Vessels = new Dictionary<uint, ClientVessel>();
}


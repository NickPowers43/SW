using UnityEngine;
using UnityEngine.Networking;
using System.Collections;

public class MyProximitySensor : NetworkProximityChecker {

	public override bool OnCheckObserver(NetworkConnection newObserver)
	{

		return false;
	}
}

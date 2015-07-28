using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Collections.Generic;

public class MyProximitySensor : NetworkProximityChecker {

	public override bool OnCheckObserver(NetworkConnection newObserver)
	{

		return false;
	}


}

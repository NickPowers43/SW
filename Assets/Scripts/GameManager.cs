using UnityEngine;
using System.Collections;

public class GameManager : MonoBehaviour {

	private QTSpaceAllocator spaceQT;
	
	private static GameManager instance = null;
	public static GameManager Instance
	{
		get{
			return instance;
		}
		set{
			instance = value;
		}
	}

	void Start () {

		if (instance == null) {
			Instance = this;
		} else {
			DestroyImmediate(this);
			return;
		}

		GlobalVariables.CharacterInterior = LayerMask.NameToLayer("CharacterInterior");
		GlobalVariables.CharacterExterior = LayerMask.NameToLayer("CharacterExterior");

		spaceQT = new QTSpaceAllocator(17);
	}

	void Update () {
	
	}

	public Vessel CreateVessel()
	{
		return null;
	}
}

public class GlobalVariables
{
	private static int characterInterior;
	public static int CharacterInterior
	{
		get{
			return characterInterior;
		}
		set{
			characterInterior = value;
		}
	}
	private static int characterExterior;
	public static int CharacterExterior
	{
		get{
			return characterExterior;
		}
		set{
			characterExterior = value;
		}
	}
}

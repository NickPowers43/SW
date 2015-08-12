using UnityEngine;
using UnityEngine.Networking;
using System.Collections;

public class GameManager : MonoBehaviour {

	public void SelectBlockType(int type)
	{
		Debug.Log("Selecting block type: " + type.ToString());
	}

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
	
	public GameObject wallNodePrefab;
	public GameObject wallSmallPrefab;
	public GameObject wallMediumPrefab;
	public GameObject wallLargePrefab;
	public GameObject chunkPrefab;

	public GameManager()
	{
		if (instance == null) {
			Instance = this;
		} else {
			//DestroyImmediate(this);
			return;
		}
	}

	void Start () {

		GlobalVariables.CharacterInterior = LayerMask.NameToLayer("CharacterInterior");
		GlobalVariables.CharacterExterior = LayerMask.NameToLayer("CharacterExterior");

		spaceQT = new QTSpaceAllocator(17);
	}

	void Update ()
	{
		if (Network.isServer) {

			foreach (var vessel in ServerVessel.Vessels) {

				vessel.Update();

			}
		}
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

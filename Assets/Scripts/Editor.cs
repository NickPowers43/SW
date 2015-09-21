using UnityEngine;
using System.Collections;

public class Editor : MonoBehaviour {

	public static int characterLM;
	public static int vesselLM;

	public GameObject wallNodePrefab;
	public GameObject wallSmallPrefab;
	public GameObject wallMediumPrefab;
	public GameObject wallLargePrefab;

	void Start () {

		CharacterLM = 1 << LayerMask.NameToLayer("Character");
		VesselLM = 1 << LayerMask.NameToLayer("Vessel");
	}
	
	// Update is called once per frame
	void Update () {

		if (Input.GetKeyDown(KeyCode.U)) {
			//create new vessel

			//Vessel vessel = GameObject.Instantiate(vesselRootPrefab, cursorWorldPosition, Quaternion.identity) as Vessel;

		}

		if (Input.GetMouseButtonDown(0)) {

			//Vessel vesselGO = Physics2D.OverlapCircle((Vector2)CursorWorldPosition, 0.01f, VesselLM).gameObject.GetComponent<Vessel>();


		}
	}
	
	public static int CharacterLM
	{
		get{
			return characterLM;
		}
		set{
			characterLM = value;
		}
	}
	public static int VesselLM
	{
		get{
			return vesselLM;
		}
		set{
			vesselLM = value;
		}
	}
}

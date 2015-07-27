using UnityEngine;
using System.Collections;

public class Editor : MonoBehaviour {
	
	private static Vector2 cursorWorldPosition = new Vector2(0.0f, 0.0f);

	private static int characterLM;
	private static int vesselLM;

	public GameObject wallNodePrefab;
	public GameObject wallSmallPrefab;
	public GameObject wallMediumPrefab;
	public GameObject wallLargePrefab;

	public GameObject vesselRootPrefab;

	private Camera camera;

	void Start () {
		camera = GetComponent<Camera>();

		CharacterLM = 1 << LayerMask.NameToLayer("Character");
		VesselLM = 1 << LayerMask.NameToLayer("Vessel");
	}
	
	// Update is called once per frame
	void Update () {

		CursorWorldPosition = (Vector2)camera.ScreenToWorldPoint(Input.mousePosition);

		if (Input.GetKeyDown(KeyCode.U)) {
			//create new vessel

			//Vessel vessel = GameObject.Instantiate(vesselRootPrefab, cursorWorldPosition, Quaternion.identity) as Vessel;

		}

		if (Input.GetMouseButtonDown(0)) {

			//Vessel vesselGO = Physics2D.OverlapCircle((Vector2)CursorWorldPosition, 0.01f, VesselLM).gameObject.GetComponent<Vessel>();


		}
	}
	
	public static Vector2 CursorWorldPosition
	{
		get
		{
			return cursorWorldPosition;
		}
		set
		{
			cursorWorldPosition = value;
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

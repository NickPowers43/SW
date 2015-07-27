using UnityEngine;
using System.Collections;

public class MainCamera : MonoBehaviour {

	private static MainCamera instance = null;
	public static MainCamera Instance
	{
		get{
			return instance;
		}
		set{
			instance = value;
		}
	}

	private Vector3 target;
	public Camera camera;

	void Start () {

		if (instance == null) {
			Instance = this;
		} else {
			DestroyImmediate(this);
			return;
		}

		camera = GetComponent<Camera>();
	}

	void Update () {

	}

	void FixedUpdate () {
		
		Rigidbody2D rb = GetComponent<Rigidbody2D>();
		
		Vector3 offset = transform.position - target;
		
		if (offset.magnitude < 10.0f) {
			rb.AddForce((Vector2)offset);
		} else {
			transform.position = target + new Vector3(0.0f,0.0f,-10.0f);
		}
		
	}

	public void SetTarget(Vector3 target)
	{
		this.target = target;
	}
}

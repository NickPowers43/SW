using UnityEngine;
using System.Collections;
using Utility;

public class Player : MonoBehaviour
{
	public static float OBSERVER_UPDATE_INTERVAL = 2.0f;
	public static float NETWORK_VIS_RANGE_SQR = 15.0f * 15.0f;

	public ushort id;
	public float maxSpeed = 5.5f;
	private Vec2i chunkI;
	public Vec2i ChunkI
	{
		get {
			return chunkI;
		}
		set {
			chunkI = value;
		}
	}
	public Rigidbody2D rb;

	private float baseRot;


	void Start()
	{
		rb = GetComponent<Rigidbody2D>();
		baseRot = transform.rotation.eulerAngles.z;
	}

	void Update()
	{

	}

	public void Move(Vector2 move, Vector2 facing)
	{
		Quaternion q = new Quaternion();
		float angle = (Mathf.Rad2Deg * Mathf.Atan2(facing.y, facing.x));
		q.eulerAngles = new Vector3(0.0f, 0.0f, angle + baseRot);
		transform.rotation = q;
		
		// Move the character
		rb.velocity = move * maxSpeed;
		
	}
}


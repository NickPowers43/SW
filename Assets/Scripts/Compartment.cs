using UnityEngine;
using System;


public class Compartment
{
	public float area;
	public float pressure;
	public float oxygen;
	public Color color;

	public Compartment Instance
	{
		get{
			if (isPtr) {
				return ptrValue;
			} else {
				return this;
			}
		}
	}

	public bool isPtr;
	public Compartment ptrValue;

	public Compartment ()
	{
		color = new Color(UnityEngine.Random.value, UnityEngine.Random.value, UnityEngine.Random.value);
		isPtr = false;
		ptrValue = null;
	}

	public void Combine(Compartment other)
	{
		if (other != this) {
			isPtr = true;
			ptrValue = other.Instance;
		}
	}

//	public static bool operator == (Compartment a, Compartment b)
//	{
//		return 
//			a == b || 
//			(a.isPtr && a.ptrValue == b) ||
//			(b.isPtr && b.ptrValue == a) ||
//			((a.isPtr && b.isPtr) && a.ptrValue == b.ptrValue);
//	}
}


using System;


public class Compartment
{
	public float area;
	public float pressure;
	public float oxygen;

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
		isPtr = false;
		ptrValue = null;
	}

	public void Combine(Compartment other)
	{
		isPtr = true;
		ptrValue = other;
	}

	public static bool operator == (Compartment a, Compartment b)
	{
		return 
			a == b || 
			(a.isPtr && a.ptrValue == b) ||
			(b.isPtr && b.ptrValue == a) ||
			((a.isPtr && b.isPtr) && a.ptrValue == b.ptrValue);
	}
}


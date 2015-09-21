
using UnityEngine;

public struct SimpleTimer
{
	private float duration;
	private float elapsed;
	
	public SimpleTimer(float duration)
	{
		this.duration = duration;
		elapsed = 0.0f;
	}
	
	public bool Poll()
	{
		return elapsed >= duration;
	}
	
	public bool Update()
	{
		if (Poll())
		{
			return true;
		}
		else
		{
			elapsed += Time.deltaTime;
			return false;
		}
	}
	
	public void Set(float duration)
	{
		this.duration = duration;
		this.elapsed = 0.0f;
	}
}


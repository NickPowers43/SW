using UnityEngine;
using System;

namespace Utility
{
	public struct Vec2i
	{
		public int x;
		public int y;
		
		public Vec2i (int x, int y)
		{
			this.x = x;
			this.y = y;
		}

		public Vec2i Abs()
		{
			return new Vec2i(Mathf.Abs(x), Mathf.Abs(y));
		}

		public static Vec2i operator * (Vec2i a, Vec2i b)
		{
			return new Vec2i (a.x * b.x, a.y * b.y);
		}
		public static Vec2i operator / (Vec2i a, Vec2i b)
		{
			return new Vec2i (a.x / b.x, a.y / b.y);
		}
		public static Vec2i operator / (Vec2i a, int b)
		{
			return new Vec2i (a.x / b, a.y / b);
		}
		public static Vec2i operator * (Vec2i a, int b)
		{
			return new Vec2i (a.x * b, a.y * b);
		}
		public static Vec2i operator + (Vec2i a, Vec2i b)
		{
			return new Vec2i (a.x + b.x, a.y + b.y);
		}
		public static Vec2i operator + (Vec2i a, int b)
		{
			return new Vec2i (a.x + b, a.y + b);
		}
		public static Vec2i operator - (Vec2i a, Vec2i b)
		{
			return new Vec2i (a.x - b.x, a.y - b.y);
		}
		public static Vec2i operator - (Vec2i a, int b)
		{
			return new Vec2i (a.x - b, a.y - b);
		}
		public static Vec2i operator << (Vec2i a, int b)
		{
			return new Vec2i (a.x << b, a.y << b);
		}

		public static bool operator < (Vec2i a, int b)
		{
			return Mathf.Abs(a.x) < b && Mathf.Abs(a.y) < b;
		}
		public static bool operator <= (Vec2i a, int b)
		{
			return Mathf.Abs(a.x) <= b && Mathf.Abs(a.y) <= b;
		}
		public static bool operator > (Vec2i a, int b)
		{
			return Mathf.Abs(a.x) > b && Mathf.Abs(a.y) > b;
		}
		public static bool operator >= (Vec2i a, int b)
		{
			return Mathf.Abs(a.x) >= b && Mathf.Abs(a.y) >= b;
		}

		public static Vec2i operator >> (Vec2i a, int b)
		{
			return new Vec2i (a.x >> b, a.y >> b);
		}
		public static bool operator == (Vec2i a, Vec2i b)
		{
			return a.x == b.x && a.y == b.x;
		}
		public static bool operator != (Vec2i a, Vec2i b)
		{
			return a.x != b.x || a.y != b.x;
		}
		public static explicit operator Vector2(Vec2i a)
		{
			return new Vector2((float)a.x, (float)a.y);
		}

		public string ToString()
		{
			return "(" + this.x + ", " + this.y + ")";
		}
	}
}


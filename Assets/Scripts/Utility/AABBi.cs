using UnityEngine;
using System;

namespace Utility
{
	public struct AABBi
	{
		public Vec2i bl, tr;
		
		public AABBi (Vec2i bl, Vec2i tr)
		{
			this.bl = bl;
			this.tr = tr;
		}

		public void FitWhole(Vec2i point)
		{
			if (point.x < bl.x){
				bl.x = point.x;
			}
			if (point.y < bl.y){
				bl.y = point.y;
			}
			
			if (point.x >= tr.x){
				tr.x = point.x + 1;
			}
			if (point.y >= tr.y){
				tr.y = point.y + 1;
			}
		}
		
		public void Fit(Vec2i point)
		{
			if (point.x < bl.x){
				bl.x = point.x;
			}
			if (point.y < bl.y){
				bl.y = point.y;
			}
			
			if (point.x > tr.x){
				tr.x = point.x;
			}
			if (point.y > tr.y){
				tr.y = point.y;
			}
		}

		public void FitAABB(ref AABBi aabb)
		{
			bl.x = Mathf.Min(aabb.bl.x, bl.x);
			bl.y = Mathf.Min(aabb.bl.y, bl.y);

			tr.x = Mathf.Max(aabb.tr.x, tr.x);
			tr.y = Mathf.Max(aabb.tr.y, tr.y);
		}
	}
}




using System;
using System.Collections.Generic;

namespace Utility
{
	public class DynamicArray2D<T> where T : class
	{
		public Vec2i origin;
		public Vec2i dim;
		public T[] data;
		
		public DynamicArray2D()
		{
			this.origin = new Vec2i(0, 0);
			this.dim = new Vec2i(0, 0);
			this.data = new T[0];
		}

		public void AABBNonDef(out Vec2i bl, out Vec2i tr)
		{
			bl = new Vec2i(dim.x,dim.y);
			tr = new Vec2i(0,0);

			for (int i = 0; i < dim.y; i++) {
				for (int j = 0; j < dim.x; j++) {

					if (data [indexOf(j,i)] != null) {

						if (j < bl.x){
							bl.x = j;
						}
						if (i < bl.y){
							bl.y = i;
						}

						if (j >= tr.x){
							tr.x = j + 1;
						}
						if (i >= tr.y){
							tr.y = i + 1;
						}
					}

				}
			}

			bl += origin;
			tr += origin;

			if (bl.x > tr.x) {
				tr.x = bl.x;
			}
			if (bl.y > tr.y) {
				tr.y = bl.y;
			}
		}
		
		public IEnumerable<T> TryGetOutsideInside(Vec2i outside, Vec2i inside, int range)
		{
			int rangeT = (range * 2) + 1;

			for (int i = 0; i < rangeT; i++) {
				for (int j = 0; j < rangeT; j++) {

					Vec2i temp = inside + new Vec2i(i - range, j - range);

					if (!((temp - outside) <= range)) {
						T temp2 = null;
						if (TryGet (temp.x, temp.y, ref temp2)) {
							yield return TryGet(temp.x, temp.y);
						}
					}
				}
			}
		}
		
		public IEnumerable<T> WithinRange(Vec2i inside, int range)
		{
			int rangeT = (range * 2) + 1;
			
			for (int i = 0; i < rangeT; i++) {
				for (int j = 0; j < rangeT; j++) {
					
					Vec2i temp = inside + new Vec2i(i - range, j - range);
					
					T temp2 = null;
					if (TryGet (temp.x, temp.y, ref temp2)) {
						yield return TryGet(temp.x, temp.y);
					}
				}
			}
		}

		private void GrowTopRight(Vec2i amount)
		{
			Vec2i newDim = dim + amount;
			T[] newData = new T[newDim.x * newDim.y];

			for (int i = 0; i < amount.y; i++) {
				for (int j = 0; j < newDim.x; j++) {
					newData[indexOf(j,newDim.y - i, newDim.x)] = null;
				}
			}

			for (int i = 0; i < dim.y; i++) {
				for (int j = 0; j < dim.x; j++) {
					newData[indexOf(j,i, newDim.x)] = data[indexOf(j,i, dim.x)];
				}
				for (int j = dim.x; j < newDim.x; j++) {
					newData[indexOf(j,i, newDim.x)] = null;
				}
			}

			data = newData;
			dim = newDim;
		}
		
		private void GrowBottomLeft(Vec2i amount)
		{
			
			Vec2i newDim = dim + amount;
			T[] newData = new T[newDim.x * newDim.y];
			
			for (int i = amount.y; i < newDim.y; i++) {
				for (int j = 0; j < amount.x; j++) {
					newData[indexOf(j,i, newDim.x)] = null;
				}
				for (int j = amount.x; j < newDim.x; j++) {
					newData[indexOf(j,i, newDim.x)] = data[indexOf(j - amount.x,i - amount.y, dim.x)];
				}
			}
			
			for (int i = 0; i < amount.y; i++) {
				for (int j = 0; j < newDim.x; j++) {
					newData[indexOf(j,i, newDim.x)] = null;
				}
			}

			origin -= amount;
			data = newData;
			dim = newDim;
		}

		public bool Contains(int x, int y)
		{
			return (x >= origin.x && x < dim.x + origin.x) && (y >= origin.y && y < dim.y + origin.y);
		}
		
		public bool Contains(Vec2i index)
		{
			return (index.x >= origin.x && index.x < dim.x + origin.x) && (index.y >= origin.y && index.y < dim.y + origin.y);
		}
		
		public int indexOf (Vec2i index)
		{
			return (index.y * dim.x) + index.x;
		}
		
		public static int indexOf (Vec2i index, int columns)
		{
			return (index.y * columns) + index.x;
		}
		
		public int indexOf (int x, int y)
		{
			return (y * dim.x) + x;
		}
		
		public static int indexOf (int x, int y, int columns)
		{
			return (y * columns) + x;
		}
		
		public void Set (int x, int y, T val)
		{
			if (x < origin.x) {
				if (y < origin.y) {
					//expand bottom left
					GrowBottomLeft(new Vec2i(origin.x - x, origin.y - y));
				} else if (y >= dim.y + origin.y) {
					//expand top left
					GrowTopRight(new Vec2i(0, y - dim.y + origin.y + 1));
					GrowBottomLeft(new Vec2i(origin.x - x, 0));
				} else {
					//expand left
					GrowBottomLeft(new Vec2i(origin.x - x, 0));
				}
			} else if (x >= dim.x + origin.x) {
				if (y < origin.y) {
					//expand bottom right
					GrowTopRight(new Vec2i(x - dim.x + origin.x + 1, 0));
					GrowBottomLeft(new Vec2i(0, y - dim.y + origin.y + 1));
				} else if (y >= dim.y + origin.y) {
					//expand top right
					GrowTopRight(new Vec2i(x - dim.x + origin.x + 1, y - dim.y + origin.y + 1));
				} else {
					//expand right
					GrowTopRight(new Vec2i(x - dim.x + origin.x + 1, 0));
				}
			} else {
				if (y < origin.y) {
					//expand bottom
					GrowBottomLeft(new Vec2i(0, origin.y - y));
				} else if (y >= dim.y + origin.y) {
					//expand top
					GrowTopRight(new Vec2i(0, y - dim.y + origin.y + 1));
				}
				else {
					x -= origin.x;
					y -= origin.y;
					data [indexOf(x,y)] = val;
				}
			}
		}
		
		public bool TryGet (int x, int y, ref T val)
		{
			if (!Contains(x, y))
				return false;
			
			x -= origin.x;
			y -= origin.y;
			val = data [indexOf(x,y)];

			return true;
		}
		
		public T TryGet (int x, int y)
		{
			if (!Contains(x, y))
				return null;

			x -= origin.x;
			y -= origin.y;
			return data [indexOf(x,y)];
		}
		
		public T TryGet (Vec2i index)
		{
			if (!Contains(index.x, index.y))
				return null;
			
			index -= origin;
			return data [indexOf(index)];
		}
		
		public T Get (int x, int y)
		{
			x -= origin.x;
			y -= origin.y;
			return data [indexOf(x,y)];
		}
		
		public T Get2 (int x, int y)
		{
			return data [indexOf(x,y)];
		}

//		public void Set (Vec2i index, T val)
//		{
//			
//			data [(index.y * dim.x) + index.x] = val;
//		}
//		
//		public T Get (Vec2i index)
//		{
//			return data [(index.y * dim.x) + index.x];
//		}
	}
}


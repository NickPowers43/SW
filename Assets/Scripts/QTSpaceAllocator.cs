using UnityEngine;
using System;

public class QTSpaceAllocator
{
	public static float MINIMUM_SIZE = 8.0f;
	private QTNode root;

	public QTSpaceAllocator (int levels)
	{
		root = new QTNode(MINIMUM_SIZE * (1 << levels), Vector2.zero);
	}

	public bool AllocateSpace(Vector2 size, ref ulong address, ref Rect space)
	{
		address = 0;

		if (size.x > root.size || size.x > root.size) {
			return false;
		}

		return root.AllocateSpace(size, ref address, ref space);
	}
	
	public void FreeSpace(ulong address)
	{
		root.FreeSpace(ref address);
	}

	private class QTNode
	{
		public bool allocated;
		public float size;
		public Vector2 origin;

		QTNode bl;
		QTNode tl;
		QTNode br;
		QTNode tr;

		public QTNode (float size, Vector2 origin)
		{
			this.size = size;
			this.origin = origin;

			this.bl = null;
			this.tl = null;
			this.br = null;
			this.tr = null;
		}
		
		private static bool Empty(QTNode node)
		{
			return node == null || node.Empty();
			
		}

		public bool Empty()
		{
			bool output = true;

			output &= bl != null || bl.Empty();
			output &= tl != null || tl.Empty();
			output &= br != null || br.Empty();
			output &= tr != null || tr.Empty();

			output &= !allocated;

			return output;
		}
		
		public void FreeSpace(ref ulong address)
		{
			if (allocated) {
				allocated = false;
				return;
			} else {
				int i = PopAddress(ref address);

				switch (i) {
				case 0:
					bl.FreeSpace(ref address);
					return;
				case 1:
					tl.FreeSpace(ref address);
					return; 
				case 2:
					br.FreeSpace(ref address);
					return; 
				case 3:
					tr.FreeSpace(ref address);
					return; 
				default:
					throw new ArgumentException();
				}
			}
		}

		private static int PopAddress(ref ulong address)
		{
			ulong output = address & (0xf >> 2);
			address >>= 2;
			return (int)output;
		}

		private static void PushAddress(ref ulong address, int index)
		{
			address <<= 2;
			address += (ulong)index;
		}

		public bool AllocateSpace(Vector2 size, ref ulong address, ref Rect space)
		{
			if (!allocated) {
				//check if size is bigger than children can hold
				Vector2 dim = new Vector2(this.size * 0.5f, this.size * 0.5f);

				if (size.x > dim.x || size.y > dim.y) {
					//size to big for children
					if (Empty(bl) && Empty(tl) && Empty(br) && Empty(tr)) {
						allocated = true;
						space = new Rect(origin - dim, new Vector2(this.size, this.size));
						return true;
					}
					else {
						address >>= 2;
						return false;
					}
				}
				else if (!Mathf.Approximately(this.size, MINIMUM_SIZE)) {

					float sizeHalf = this.size * 0.5f;

					//allocate space in a node that is a child of this node
					//try bottom left
					PushAddress(ref address, 0);
					if (bl == null) {
						bl = new QTNode(sizeHalf, origin - (dim * 0.5f));
						return bl.AllocateSpace(size, ref address, ref space);
					} else if (bl.AllocateSpace(size, ref address, ref space)) {
						return true;
					}
					
					PushAddress(ref address, 1);
					if (tl == null) {
						tl = new QTNode(sizeHalf, origin + new Vector2(this.size * -0.5f, this.size * 0.5f));
						return tl.AllocateSpace(size, ref address, ref space);
					} else if (tl.AllocateSpace(size, ref address, ref space)) {
						return true;
					}
					
					PushAddress(ref address, 2);
					if (br == null) {
						br = new QTNode(sizeHalf, origin + new Vector2(this.size * 0.5f, this.size * -0.5f));
						return br.AllocateSpace(size, ref address, ref space);
					} else if (br.AllocateSpace(size, ref address, ref space)) {
						return true;
					}
					
					PushAddress(ref address, 3);
					if (tr == null) {
						tr = new QTNode(sizeHalf, origin + (dim * 0.5f));
						return tr.AllocateSpace(size, ref address, ref space);
					} else if (tr.AllocateSpace(size, ref address, ref space)) {
						return true;
					} else {
						address >>= 2;
						return false;
					}
					
				}
				else {
					//allocate the space of this whole node
					allocated = true;
					space = new Rect(origin - dim, new Vector2(this.size, this.size));
					return true;
				}
			} else {
				address >>= 2;
				return false;
			}

		}
	}
}


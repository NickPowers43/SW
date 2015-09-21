#pragma once

namespace SW
{
	template<typename T>
	class LinkedListNode
	{
	public:
		LinkedListNode(T* next)
		{
			next = next;
		}
		~LinkedListNode()
		{
			if (next)
			{
				delete next;
			}
		}

		T* & Next()
		{
			
			return next;
		}

	private:
		T* next;
	};

}

#pragma once

namespace SW_Server
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

#pragma once

template<typename node_t, typename Functor>
void ll_for_each(node_t* start, Functor functor)
{
	while (start)
	{
		functor(start);
		start = start->next;
	}
}

template<typename node_t, typename Functor>
void ll_for_each(node_t** start, Functor functor)
{
	while (start)
	{
		functor(start, *start);
		start = start->next;
	}
}

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

		T* next;
	};

}

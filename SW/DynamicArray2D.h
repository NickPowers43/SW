#pragma once
#include "SW.h"

namespace SW
{
	template<typename value_type>
	class DynamicArray2D 
	{
	public:

		glm::ivec2 origin;
		glm::ivec2 dim;
		value_type** data;

		DynamicArray2D() : origin(0, 0), dim(0, 0), data(NULL)
		{

		}

		~DynamicArray2D()
		{
		}

		//template<typename value_type>
		void GrowTopRight(glm::ivec2 amount)
		{
			//UnityEngine.Debug.Log("GrowTopRight: " + amount.ToString());

			glm::ivec2 newDim = dim + amount;
			value_type** newData = new value_type*[newDim.x * newDim.y];
			for (size_t i = 0; i < newDim.x * newDim.y; i++)
			{
				newData[i] = NULL;
			}

			for (int i = 0; i < dim.y; i++) {
				for (int j = 0; j < dim.x; j++) {
					newData[(i * newDim.x) + j] = data[(i * dim.x) + j];
				}
			}

			data = newData;
			dim = newDim;
		}

		//template<typename value_type>
		void GrowBottomLeft(glm::ivec2 amount)
		{
			//UnityEngine.Debug.Log("GrowBottomLeft: " + amount.ToString());

			glm::ivec2 newDim = dim + amount;
			value_type** newData = new value_type*[newDim.x * newDim.y];
			for (size_t i = 0; i < newDim.x * newDim.y; i++)
			{
				newData[i] = NULL;
			}

			for (int i = amount.y; i < newDim.y; i++) {
				for (int j = amount.x; j < newDim.x; j++) {
					newData[(i * newDim.x) + j] = data[(i - amount.y * dim.x) + j - amount.x];
				}
			}

			origin -= amount;
			data = newData;
			dim = newDim;
		}

		//template<typename value_type>
		bool Contains(int x, int y)
		{
			return (x >= origin.x && x < dim.x + origin.x) && (y >= origin.y && y < dim.y + origin.y);
		}

		//template<typename value_type>
		bool Contains(glm::ivec2 index)
		{
			return (index.x >= origin.x && index.x < dim.x + origin.x) && (index.y >= origin.y && index.y < dim.y + origin.y);
		}

		//template<typename value_type>
		void Set(glm::ivec2 index, value_type* val)
		{
			Set(index.x, index.y, val);
		}

		//template<typename value_type>
		void Set(int x, int y, value_type* val)
		{
			if (x < origin.x) {
				if (y < origin.y) {
					//expand bottom left
					GrowBottomLeft(glm::ivec2(origin.x - x, origin.y - y));
				}
				else if (y >= dim.y + origin.y) {
					//expand top left
					GrowTopRight(glm::ivec2(0, y - (dim.y + origin.y) + 1));
					GrowBottomLeft(glm::ivec2(origin.x - x, 0));
				}
				else {
					//expand left
					GrowBottomLeft(glm::ivec2(origin.x - x, 0));
				}
			}
			else if (x >= dim.x + origin.x) {
				if (y < origin.y) {
					//expand bottom right
					GrowTopRight(glm::ivec2(x - (dim.x + origin.x) + 1, 0));
					GrowBottomLeft(glm::ivec2(0, origin.y - y));
				}
				else if (y >= dim.y + origin.y) {
					//expand top right
					GrowTopRight(glm::ivec2(x - (dim.x + origin.x) + 1, y - (dim.y + origin.y) + 1));
				}
				else {
					//expand right
					GrowTopRight(glm::ivec2(x - (dim.x + origin.x) + 1, 0));
				}
			}
			else {
				if (y < origin.y) {
					//expand bottom
					GrowBottomLeft(glm::ivec2(0, origin.y - y));
				}
				else if (y >= dim.y + origin.y) {
					//expand top
					GrowTopRight(glm::ivec2(0, y - (dim.y + origin.y) + 1));
				}
			}

			x -= origin.x;
			y -= origin.y;
			data[(y * dim.x) + x] = val;
		}

		//template<typename value_type>
		bool TryGet(int x, int y, value_type** val)
		{
			if (!Contains(x, y))
				return false;

			x -= origin.x;
			y -= origin.y;
			*val = data[(y * dim.x) + x];

			return true;
		}

		//template<typename value_type>
		value_type* TryGet(int x, int y)
		{
			if (!Contains(x, y))
				return NULL;

			x -= origin.x;
			y -= origin.y;
			return data[(y * dim.x) + x];
		}

		//template<typename value_type>
		value_type* TryGet(glm::ivec2 index)
		{
			if (!Contains(index.x, index.y))
				return NULL;

			index -= origin;
			return data[(index.y * dim.x) + index.x];
		}

		//template<typename value_type>
		value_type* Get(int x, int y)
		{
			x -= origin.x;
			y -= origin.y;
			return data[(y * dim.x) + x];
		}

		//template<typename value_type>
		value_type* Get(glm::ivec2 index)
		{
			index -= origin;
			return data[(index.y * dim.x) + index.x];
		}

		//template<typename value_type>
		value_type* Get2(int x, int y)
		{
			return data[(y * dim.x) + x];
		}

	};
}


namespace SW
{
	
}

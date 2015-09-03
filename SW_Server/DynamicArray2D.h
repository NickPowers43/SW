#pragma once

namespace SW_Server
{
	template<typename T>
	class DynamicArray2D 
	{
	public:

		glm::ivec2 origin;
		glm::ivec2 dim;
		T** data;

		DynamicArray2D();
		~DynamicArray2D();

		/*void AABBNonDef(out glm::ivec2 bl, out glm::ivec2 tr);

		IEnumerable<T*> T*ryGetOutsideInside(glm::ivec2 outside, glm::ivec2 inside, int range);

		IEnumerable<T*> WithinRange(glm::ivec2 origin, int range);*/

		void GrowTopRight(glm::ivec2 amount);
		void GrowBottomLeft(glm::ivec2 amount);
		bool Contains(int x, int y);
		bool Contains(glm::ivec2 index);
		void Set(glm::ivec2 index, T* val);
		void Set(int x, int y, T* val);
		bool TryGet(int x, int y, T** val);
		T* TryGet(int x, int y);
		T* TryGet(glm::ivec2 index);
		T* Get(int x, int y);
		T* Get(glm::ivec2 index);
		T* Get2(int x, int y);

	};
}


namespace SW_Server
{
	template<typename T>
	DynamicArray2D<typename T>::DynamicArray2D() : origin(0, 0), dim(0, 0), data(NULL)
	{
		
	}


	template<typename T>
	DynamicArray2D<typename T>::~DynamicArray2D()
	{
	}

	/*template<typename T>
	void DynamicArray2D::AABBNonDef(out glm::ivec2 bl, out glm::ivec2 tr)
	{
	bl = glm::ivec2(dim.x, dim.y);
	tr = glm::ivec2(0, 0);

	for (int i = 0; i < dim.y; i++) {
	for (int j = 0; j < dim.x; j++) {

	if (data[indexOf(j, i)] != NULL) {

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

	IEnumerable<T> TryGetOutsideInside(glm::ivec2 outside, glm::ivec2 inside, int range)
	{
	int rangeT = (range * 2) + 1;

	for (int i = 0; i < rangeT; i++) {
	for (int j = 0; j < rangeT; j++) {

	glm::ivec2 temp = inside + glm::ivec2(i - range, j - range);

	if (!((temp - outside) <= range)) {
	T temp2 = null;
	if (TryGet(temp.x, temp.y, ref temp2)) {
	yield return TryGet(temp.x, temp.y);
	}
	}
	}
	}
	}

	IEnumerable<T> WithinRange(glm::ivec2 origin, int range)
	{
	int rangeT = (range * 2) + 1;

	for (int i = 0; i < rangeT; i++) {
	for (int j = 0; j < rangeT; j++) {

	glm::ivec2 temp = origin + glm::ivec2(i - range, j - range);

	T* temp2 = null;
	if (TryGet(temp.x, temp.y, ref temp2)) {
	yield return temp2;
	}
	}
	}
	}*/

	template<typename T>
	void DynamicArray2D<typename T>::GrowTopRight(glm::ivec2 amount)
	{
		//UnityEngine.Debug.Log("GrowTopRight: " + amount.ToString());

		glm::ivec2 newDim = dim + amount;
		T** newData = new T*[newDim.x * newDim.y];
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

	template<typename T>
	void DynamicArray2D<typename T>::GrowBottomLeft(glm::ivec2 amount)
	{
		//UnityEngine.Debug.Log("GrowBottomLeft: " + amount.ToString());

		glm::ivec2 newDim = dim + amount;
		T** newData = new T*[newDim.x * newDim.y];
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

	template<typename T>
	bool DynamicArray2D<typename T>::Contains(int x, int y)
	{
		return (x >= origin.x && x < dim.x + origin.x) && (y >= origin.y && y < dim.y + origin.y);
	}

	template<typename T>
	bool DynamicArray2D<typename T>::Contains(glm::ivec2 index)
	{
		return (index.x >= origin.x && index.x < dim.x + origin.x) && (index.y >= origin.y && index.y < dim.y + origin.y);
	}

	template<typename T>
	void DynamicArray2D<typename T>::Set(glm::ivec2 index, T* val)
	{
		Set(index.x, index.y, val);
	}

	template<typename T>
	void DynamicArray2D<typename T>::Set(int x, int y, T* val)
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

	template<typename T>
	bool DynamicArray2D<typename T>::TryGet(int x, int y, T** val)
	{
		if (!Contains(x, y))
			return false;

		x -= origin.x;
		y -= origin.y;
		*val = data[(y * dim.x) + x];

		return true;
	}

	template<typename T>
	T* DynamicArray2D<typename T>::TryGet(int x, int y)
	{
		if (!Contains(x, y))
			return NULL;

		x -= origin.x;
		y -= origin.y;
		return data[(y * dim.x) + x];
	}

	template<typename T>
	T* DynamicArray2D<typename T>::TryGet(glm::ivec2 index)
	{
		if (!Contains(index.x, index.y))
			return NULL;

		index -= origin;
		return data[(index.y * dim.x) + index.x];
	}

	template<typename T>
	T* DynamicArray2D<typename T>::Get(int x, int y)
	{
		x -= origin.x;
		y -= origin.y;
		return data[(y * dim.x) + x];
	}

	template<typename T>
	T* DynamicArray2D<typename T>::Get(glm::ivec2 index)
	{
		index -= origin;
		return data[(index.y * dim.x) + index.x];
	}

	template<typename T>
	T* DynamicArray2D<typename T>::Get2(int x, int y)
	{
		return data[(y * dim.x) + x];
	}
}

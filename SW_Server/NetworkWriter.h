
#define MB_CAPACITY 16384
struct NetworkWriter
{
	NetworkWriter();
	~NetworkWriter();

	int size;
	void* data;
	void* temp;

	template<typename T> void* append_to_R(T & new_data)
	{
		if ((char*)temp - (char*)data >= MB_CAPACITY)
			throw std::exception("buffer overflow");

		void* beginning = temp;
		*((T*)temp) = new_data;
		temp = (char*)temp + sizeof(new_data);
		size += sizeof(new_data);
		return beginning;
	}
	template<typename T> void* append_to_V(T new_data)
	{
		if ((char*)temp - (char*)data >= MB_CAPACITY)
			throw std::exception("buffer overflow");

		void* beginning = temp;
		*((T*)temp) = new_data;
		temp = (char*)temp + sizeof(new_data);
		size += sizeof(new_data);
		return beginning;
	}

	void reset();
};
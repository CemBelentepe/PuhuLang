#pragma once
#include <cstdlib>

template<class T>
class List
{
private:
	size_t count;
	size_t capacity;
	T* data;
	const double resize_factor = 1.5;
public:
	List()
		:count(0), capacity(0), data(nullptr)
	{}

	void push_back(T value)
	{
		if (count + 1 > capacity)
		{
			capacity = (capacity < 8) ? 8 : capacity * resize_factor;
			if (data != nullptr)
			{
				data = (T*)realloc(data, capacity * sizeof(T));
			}
		}

		data[++count] = value;
	}
};
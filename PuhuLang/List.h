#pragma once
#include <cstdlib>
#include <cstring>

template<class T>
class List
{
private:
	size_t m_count;
	size_t m_capacity;
	size_t m_old_cap;
	T* m_data;
	const int resize_factor = 2;
	const int min_cap = 128;
public:
	List()
		:m_count(0), m_capacity(0), m_data(nullptr)
	{}

	~List()
	{
		delete m_data;
	}

	inline size_t count() const
	{
		return m_count;
	}

	inline size_t capacity() const
	{
		return m_capacity;
	}

	inline void push_back(T value)
	{
		if (m_count + 1 > m_capacity)
		{
			m_old_cap = m_capacity;
			m_capacity = (m_capacity < min_cap) ? min_cap : m_capacity * resize_factor;
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}

		m_data[m_count++] = value;
	}

	inline T pop_back()
	{
		if (m_count < m_old_cap)
		{
			m_capacity = m_old_cap;
			m_old_cap = (m_capacity < min_cap) ? 0 : (m_capacity / resize_factor);
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}
		return m_data[--m_count];
	}

	inline T& peek()
	{
		return m_data[m_count - 1];
	}

	inline void pop_sized(size_t size)
	{
		m_count -= size;
		if (m_count < m_old_cap)
		{
			m_capacity = m_old_cap;
			m_old_cap = (m_capacity < min_cap) ? 0 : (m_capacity / resize_factor);
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}
	}

	inline T* pop_sized_ret(size_t size)
	{
		m_count -= size;
		T* val = new T[size];
		memcpy(val, &m_data[m_count], size * sizeof(T));
		if (m_count < m_old_cap)
		{
			m_capacity = m_old_cap;
			m_old_cap = (m_capacity < min_cap) ? 0 : (m_capacity / resize_factor);
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}
		return val;
	}

	template<class U>
	inline U pop_as()
	{
		m_count -= sizeof(U);
		if (m_count < m_old_cap)
		{
			m_capacity = m_old_cap;
			m_old_cap = (m_capacity < min_cap) ? 0 : (m_capacity / resize_factor);
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}
		return *(U*)(&m_data[m_count]);
	}

	template<class U>
	inline void push_as(U value)
	{
		if (m_count + sizeof(U) > m_capacity)
		{
			m_old_cap = m_capacity;
			m_capacity = (m_capacity < min_cap) ? min_cap : m_capacity * resize_factor;
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}

		(*(U*)(&m_data[m_count])) = value;
		m_count += sizeof(U);
	}

	inline void push_sized(T* value, size_t size)
	{
		if (m_count + size > m_capacity)
		{
			m_old_cap = m_capacity;
			m_capacity = (m_capacity < min_cap) ? min_cap : m_capacity * resize_factor;
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}

		memcpy(&m_data[m_count], value, size * sizeof(T));
		m_count += size;
	}

	template<class U>
	inline U peek_as()
	{
		return *(U*)(&m_data[m_count - sizeof(U)]);
	}

	template<class U>
	inline U* peek_as_ref()
	{
		return (U*)(&m_data[m_count - sizeof(U)]);
	}

	inline T* peek_sized(size_t size)
	{
		return (T*)&m_data[m_count - size];
	}

	inline void set_sized(T* value, size_t pos, size_t size)
	{
		memcpy(&m_data[pos], value, size * sizeof(T));
	}

	inline T* get_at_ref(size_t pos)
	{
		return &m_data[pos];
	}

	inline void resize(size_t new_size)
	{
		m_count = new_size;
		if (m_count < m_old_cap)
		{
			m_capacity = m_old_cap;
			m_old_cap = (m_capacity < min_cap) ? 0 : (m_capacity / resize_factor);
			m_data = (T*)realloc(m_data, m_capacity * sizeof(T));
		}
	}

	inline T& operator[](size_t i)
	{
		return m_data[i];
	}

	inline const T at(size_t i) const
	{
		return m_data[i];
	}

};
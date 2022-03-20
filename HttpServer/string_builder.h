#pragma once

#include <stdlib.h>
#include <memory>

template <typename _CharT>
class BasicStringBuilder;

using StringBuilder = BasicStringBuilder<char>;
using WStringBuilder = BasicStringBuilder<wchar_t>;

template <typename _CharT>
class BasicStringBuilder
{
private:
	//std::vector<_CharT> m_chars;
	_CharT *m_buf;
	size_t m_capacity;
	size_t m_length;

	inline void RequestCapacity(size_t cap)
	{
		if (cap == 0) cap = 1;
		if (cap > m_capacity)
		{
			m_capacity = cap;
			m_buf = (_CharT *)realloc(m_buf, cap * sizeof(_CharT));
		}
	}
public:
	inline BasicStringBuilder() :
		m_buf(nullptr), m_capacity(0), m_length(0)
	{
		RequestCapacity(64);
	}

	// creates a new builder with the desired capacity to be reserved
	inline BasicStringBuilder(size_t reserved) :
		m_buf(nullptr), m_capacity(0), m_length(0)
	{
		RequestCapacity(reserved);
	}

	// creates a new string builder from another string builder by shifting the data
	// from source over by shift elements
	inline BasicStringBuilder(const BasicStringBuilder<_CharT> &source, size_t shift) :
		m_buf(nullptr), m_capacity(0), m_length(0)
	{
		RequestCapacity(source.m_capacity);
		if (shift <= source.m_length)
			Append(source.GetElements() + shift, source.Size() - shift);
	}

	inline BasicStringBuilder(const BasicStringBuilder<_CharT> &source) :
		m_buf(nullptr), m_capacity(0), m_length(0)
	{
		m_buf = malloc(source.m_capacity * sizeof(_CharT));
		if (m_buf)
		{
			m_capacity = source.m_capacity;
			m_length = source.m_length;
			memcpy(m_buf, source.m_buf, source.m_length * sizeof(_CharT));
		}
	}

	inline ~BasicStringBuilder()
	{
		if (m_buf)
			free(m_buf);
	}

	// clears the data in the builder
	constexpr void Clear()
	{
		m_length = 0;
	}

	// append a character
	inline BasicStringBuilder<_CharT> &Append(const _CharT &c)
	{
		if (m_length == m_capacity)
			RequestCapacity(m_capacity * 2);
		m_buf[m_length] = c;
		m_length++;
		return *this;
	}

	// append a string
	inline BasicStringBuilder<_CharT> &Append(const _CharT *chars)
	{
		while (*chars)
		{
			Append(*chars);
			chars++;
		}
		return *this;
	}

	// append memory
	inline BasicStringBuilder<_CharT> &Append(const _CharT *chars, size_t count)
	{
		if (count > 0)
		{
			size_t requiredcap = m_length + count;
			RequestCapacity(requiredcap);
			memcpy(m_buf + m_length, chars, count * sizeof(_CharT));
			m_length += count;
		}
		return *this;
	}

	// append memory
	template <size_t _Count>
	inline BasicStringBuilder<_CharT> &Append(const _CharT(&chars)[_Count])
	{
		return Append(chars, _Count);
	}

	inline void ShiftBack(size_t count)
	{
		memcpy(m_buf, m_buf + count, m_length - count);
		m_length -= count;
	}

	// the number of elements stored in the string
	constexpr size_t Size() const
	{
		return m_length;
	}

	// the number of elements that can be added before resizing
	constexpr size_t Capacity() const
	{
		return m_capacity;
	}

	// returns raw elements
	constexpr _CharT *GetElements()
	{
		return m_length == 0 ? nullptr : m_buf;
	}

	// returns raw elements
	inline const _CharT *GetElements() const
	{
		return m_length == 0 ? nullptr : m_buf;
	}

	// allocates space on the heap for a null-terminated string of length Size()
	// (total bytes Size()+1). Use delete[] operator to deallocate
	inline _CharT *ToHeapString() const
	{
		_CharT *result = new _CharT[m_length + 1];
		memcpy(result, m_buf, m_length * sizeof(_CharT));
		result[m_length] = 0;
		return result;
	}

	// in-place null terminated string, subsequent calls to Append will invalidate the value
	inline const _CharT *ToInPlaceString()
	{
		Append((_CharT)0);
		m_length--;
		return m_buf;
	}

	// move operator
	inline StringBuilder &operator=(StringBuilder &&other) noexcept
	{
		if (this != &other)
		{
			m_buf = other.m_buf;
			other.m_buf = 0;
			other.m_length = 0;
			other.m_capacity = 0;
		}
		return *this;
	}

	// assignment operator
	inline StringBuilder &operator=(const StringBuilder &other)
	{
		if (this != &other)
		{
			free(m_buf);
			m_capacity = 0;
			m_length = 0;
			
			m_buf = malloc(other.m_capacity);
			if (m_buf)
			{
				memcpy(m_buf, other.m_buf, other.m_length * sizeof(_CharT));
				m_capacity = other.m_capacity;
				m_length = other.m_length;
			}
		}
		return *this;
	}
};
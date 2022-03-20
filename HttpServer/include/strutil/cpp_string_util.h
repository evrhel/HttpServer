#pragma once

#include "string_util.h"

#include <string>

namespace strutil
{
	constexpr bool equalsIgnoreCase(const std::string &first, const std::string &second)
	{
		return equalsIgnoreCase(first.c_str(), second.c_str());
	}

	constexpr bool equalsIgnoreCase(const std::string &first, const char *second)
	{
		return equalsIgnoreCase(first.c_str(), second);
	}

	constexpr bool equalsIgnoreCase(const char *first, const std::string &second)
	{
		return equalsIgnoreCase(first, second.c_str());
	}

	inline std::string &transformLowercase(std::string &string)
	{
		for (auto it = string.begin(); it != string.end(); it++)
			*it = std::tolower(*it);
		return string;
	}

	inline std::string toLowercase(const std::string &string)
	{
		std::string result = string;
		transformLowercase(result);
		return result;
	}

	inline std::string toLowercase(const char *string)
	{
		std::string result = string;
		transformLowercase(result);
		return result;
	}

	inline std::string &transformUppercase(std::string &string)
	{
		for (auto it = string.begin(); it != string.end(); it++)
			*it = std::toupper(*it);
		return string;
	}

	inline std::string toUppercase(const std::string &string)
	{
		std::string result = string;
		transformUppercase(result);
		return result;
	}

	inline std::string toUppercase(const char *string)
	{
		std::string result = string;
		transformUppercase(result);
		return result;
	}

	inline std::string &transformChars(std::string &string, char target, char replace)
	{
		for (auto it = string.begin(); it != string.end(); it++)
		{
			if (*it == target)
				*it = replace;
		}
		return string;
	}

	inline std::string replaceChars(const std::string &string, char target, char replace)
	{
		std::string result = string;
		transformChars(result, target, replace);
		return result;
	}

	inline size_t matches(const std::string &src, const std::string &match)
	{
		return matches(src.c_str(), match.c_str());
	}

	inline size_t matches(const std::string &src, const char *match)
	{
		return matches(src.c_str(), match);
	}

	inline size_t matches(const char *src, const std::string &match)
	{
		return matches(src, match.c_str());
	}

	inline size_t matchesIgnoreCase(const std::string &src, const std::string &match)
	{
		return matchesIgnoreCase(src.c_str(), match.c_str());
	}

	inline size_t matchesIgnoreCase(const std::string &src, const char *match)
	{
		return matchesIgnoreCase(src.c_str(), match);
	}

	inline size_t matchesIgnoreCase(const char *src, const std::string &match)
	{
		return matchesIgnoreCase(src, match.c_str());
	}

	template <class CharT>
	constexpr size_t readFloats(const std::basic_string<CharT> &src, float *dest, size_t count)
	{
		size_t read = 0;
		size_t off = 0;
		for (; read < count; read++)
		{
			size_t find = src.find_first_of(' ', off);
			dest[read] = (float)atof(src.substr(off, find).c_str());
			off = find + 1;
			if (find == -1)
			{
				read++;
				break;
			}
		}
		for (size_t off = read; off < count; off++)
			dest[off] = 0;
		return read;
	}

	template <class CharT, size_t _Count>
	constexpr size_t readFloats(const std::basic_string<CharT> &src, float (&dest)[_Count])
	{
		return readFloats(src, dest, _Count);
	}

	constexpr size_t readInts(const std::string &src, int *dest, size_t count)
	{
		size_t read = 0;
		size_t off = 0;
		for (; read < count; read++)
		{
			size_t find = src.find_first_of(' ', off);
			dest[read] = atoi(src.substr(off, find).c_str());
			off = find + 1;
			if (find == -1)
			{
				read++;
				break;
			}
		}
		for (size_t off = read; off < count; off++)
			dest[off] = 0;
		return read;
	}

	template <size_t _Count>
	constexpr size_t readInts(const std::string &src, int (&dest)[_Count])
	{
		return readInts(src, dest, _Count);
	}

	/*
	String value which, when compared to another, ignores case when testing equality
	while still storing the original value. Hash functions will hash to the same value
	if case insensitively equal as well. This allows for uses in maps.
	*/
	class __declspec(dllexport) CaseInsensitiveString
	{
	private:
#pragma warning(suppress : 4251)
		std::string m_value;
	public:
		CaseInsensitiveString(const std::string &value) : m_value(value) { }
		CaseInsensitiveString(const char *value) : m_value(value) { }
		CaseInsensitiveString() : m_value() { }

		constexpr const std::string &value() const
		{
			return m_value;
		}

		inline size_t length() const
		{
			return m_value.length();
		}

		inline const char *cstr() const
		{
			return m_value.c_str();
		}

		inline char at(size_t index) const
		{
			return m_value.at(index);
		}

		inline CaseInsensitiveString &operator=(const CaseInsensitiveString &other)
		{
			m_value = other.m_value;
			return *this;
		}

		inline bool operator==(const CaseInsensitiveString &other) const
		{
			return equalsIgnoreCase(m_value, other.m_value);
		}

		inline bool operator==(const std::string &other) const
		{
			return equalsIgnoreCase(m_value, other);
		}

		inline bool operator==(const char *other) const
		{
			return equalsIgnoreCase(m_value, other);
		}

		inline bool operator!=(const CaseInsensitiveString &other) const
		{
			return !equalsIgnoreCase(m_value, other.m_value);
		}

		inline bool operator!=(const std::string &other) const
		{
			return !equalsIgnoreCase(m_value, other);
		}

		inline bool operator!=(const char *other) const
		{
			return !equalsIgnoreCase(m_value, other);
		}

		inline bool operator>(const CaseInsensitiveString &other) const
		{
			return m_value > other.m_value;
		}

		inline bool operator<(const CaseInsensitiveString &other) const
		{
			return m_value < other.m_value;
		}

		inline bool operator>=(const CaseInsensitiveString &other) const
		{
			return m_value >= other.m_value;
		}

		inline bool operator<=(const CaseInsensitiveString &other) const
		{
			return m_value <= other.m_value;
		}

		inline strutil::CaseInsensitiveString &append(const strutil::CaseInsensitiveString &string)
		{
			m_value.append(string.m_value);
			return *this;
		}

		inline strutil::CaseInsensitiveString &append(const std::string &string)
		{
			m_value.append(string);
			return *this;
		}

		inline strutil::CaseInsensitiveString &append(const char *string)
		{
			m_value.append(string);
			return *this;
		}

		inline void clear()
		{
			m_value.clear();
		}
	};

	inline strutil::CaseInsensitiveString operator+(const strutil::CaseInsensitiveString &src, const strutil::CaseInsensitiveString &append)
	{
		return src.value() + append.value();
	}

	inline strutil::CaseInsensitiveString operator+(const strutil::CaseInsensitiveString &src, const std::string &append)
	{
		return src.value() + append;
	}

	inline strutil::CaseInsensitiveString operator+(const strutil::CaseInsensitiveString &src, const char *append)
	{
		return src.value() + append;
	}

	inline strutil::CaseInsensitiveString operator+(const strutil::CaseInsensitiveString &src, char append)
	{
		return src.value() + append;
	}

	constexpr bool equalsIgnoreCase(const CaseInsensitiveString &first, const CaseInsensitiveString &second)
	{
		return equalsIgnoreCase(first.cstr(), second.cstr());
	}

	constexpr bool equalsIgnoreCase(const CaseInsensitiveString &first, const char *second)
	{
		return equalsIgnoreCase(first.cstr(), second);
	}

	constexpr bool equalsIgnoreCase(const char *first, const CaseInsensitiveString &second)
	{
		return equalsIgnoreCase(first, second.cstr());
	}

	constexpr size_t readFloats(const CaseInsensitiveString &src, float *dest, size_t count)
	{
		return readFloats(src.value(), dest, count);
	}

	template <size_t _Count>
	constexpr size_t readFloats(const CaseInsensitiveString &src, float(&dest)[_Count])
	{
		return readFloats(src, dest, _Count);
	}

	constexpr size_t readInts(const CaseInsensitiveString &src, int *dest, size_t count)
	{
		return readInts(src.value(), dest, count);
	}

	template <size_t _Count>
	constexpr size_t readInts(const CaseInsensitiveString &src, int(&dest)[_Count])
	{
		return readInts(src, dest, _Count);
	}
}

namespace std
{
	template <>
	struct hash<strutil::CaseInsensitiveString>
	{
		inline size_t operator()(const strutil::CaseInsensitiveString &entry) const
		{
			return std::hash<std::string>()(strutil::toLowercase(entry.value()));
		}
	};
}

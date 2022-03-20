#pragma once

#include <cstring>
#include <cstdlib>

namespace strutil
{
	constexpr bool charEqualsIgnoreCase(char first, char second)
	{
		if (first == second) return true;
		else if (first >= 'A' && first <= 'Z') return first + 0x20 == second;
		else if (first >= 'a' && second <= 'z') return first - 0x20 == second;
		return false;
	}

	constexpr bool equals(const char *first, const char *second)
	{
		while (*first && *second)
		{
			if (*first != *second)
				return false;
			first++;
			second++;
		}
		return true;
	}

	constexpr bool equalsIgnoreCase(const char *first, const char *second)
	{
		while (*first && *second)
		{
			if (!charEqualsIgnoreCase(*first, *second)) return false;
			first++;
			second++;
		}
		return *first == *second;
	}

	constexpr char *transformLowercase(char *string)
	{
		while (*string)
		{
			if (*string <= 90 && *string >= 65)
				*string += 32;
			string++;
		}
		return string;
	}

	constexpr char *transformUppercase(char *string)
	{
		while (*string)
		{
			if (*string <= 122 && *string >= 97)
				*string -= 32;
			string++;
		}
		return string;
	}

	constexpr char *transformChars(char *string, char target, char replace)
	{
		while (*string)
		{
			if (*string == target)
				*string = replace;
			string++;
		}
		return string;
	}

	/*
	Returns whether a string matches another string. For a match
	to occur, match must be a substring of src starting at src.

	This means:
	matches("cats and dogs", "cats") would return true, but
	matches("cats and dogs", "dogs") would not.
	
	@param src The source string.
	@param match The string to match in src.

	@return true on a match and false otherwise.
	*/
	constexpr size_t matches(const char *src, const char *match)
	{
		const char *start = src;
		while (*src && *match)
		{
			if (*src != *match)
				return false;
			src++;
			match++;
		}
		if (*src == 0 && *match != 0) return -1;
		return src - start;
	}

	constexpr size_t matchesIgnoreCase(const char *src, const char *match)
	{
		const char *start = src;
		while (*src && *match)
		{
			if (!charEqualsIgnoreCase(*src, *match)) return -1;
			src++;
			match++;
		}
		if (*src == 0 && *match != 0) return -1;
		return src - start;
	}

	constexpr size_t readFloats(char *src, float *dest, size_t count)
	{
		size_t read = 0;
		char *cursor = src;
		for (; read < count; read++)
		{
			char *find = ::strchr(src, ' ');
			if (find)
			{
				*find = 0;
				dest[read] = (float)atof(cursor);
				*find = ' ';
			}
			else
			{
				dest[read] = (float)atof(cursor);
				break;
			}
			cursor = find + 1;
		}
		for (size_t off = read; off < count; off++)
			dest[off] = 0.0f;
		return read;
	}

	template <size_t _Count>
	constexpr size_t readFloats(char *src, float(&dest)[_Count])
	{
		return readFloats(src, dest, _Count);
	}

	constexpr size_t readInts(char *src, int *dest, size_t count)
	{
		size_t read = 0;
		char *cursor = src;
		for (; read < count; read++)
		{
			char *find = ::strchr(src, ' ');
			if (find)
			{
				*find = 0;
				dest[read] = ::atoi(cursor);
				*find = ' ';
			}
			else
			{
				dest[read] = ::atoi(cursor);
				break;
			}
			cursor = find + 1;
		}
		for (size_t off = read; off < count; off++)
			dest[off] = 0;
		return read;
	}

	template <size_t _Count>
	constexpr size_t readInts(char *src, int(&dest)[_Count])
	{
		return readInts(src, dest, _Count);
	}
}

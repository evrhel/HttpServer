#pragma once

#include <vector>
#include <string>

inline bool BytesEqual(const char *src, int srclen, const char *test, int testlen)
{
	if (testlen > srclen) return false;

	while (testlen > 0)
	{
		if (*src != *test) return false;

		testlen--;
		src++;
		test++;
	}

	return true;
}

inline int FindFirstOf(const char *src, int srclen, const char *test, int testlen)
{
	int ind = 0;
	while (srclen >= testlen)
	{
		if (BytesEqual(src, srclen, test, testlen))
			return ind;

		srclen--;
		src++;
		ind++;
	}
	return -1;
}

inline bool StartsWith(const char *src, int srclen, const char *test, int testlen)
{
	if (srclen < testlen) return false;
	return memcmp(src, test, testlen) == 0;
}

inline void Tokenize(char *str, const char *delim, std::vector<std::string> *dest)
{
	dest->clear();

	char *ctx;
	char *token;
	token = strtok_s(str, delim, &ctx);
	while (token)
	{
		dest->push_back(token);
		token = strtok_s(nullptr, delim, &ctx);
	}
}

inline std::string Trim(const std::string &in)
{
	size_t front, back;

	// remove from front
	for (front = 0; front < in.size(); front++)
	{
		if (in[front] != ' ' && in[front] != '\t') break;
	}

	for (back = in.size() - 1; back < in.size(); back--)
	{
		if (in[back] != ' ' && in[back] != '\t') break;
	}

	return in.substr(front, back - front + 1);
}
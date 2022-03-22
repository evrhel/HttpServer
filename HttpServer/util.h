#pragma once

#include <vector>
#include <string>

#include "string_builder.h"

enum
{
	DOW_SUN = 0,
	DOW_MON,
	DOW_TUE,
	DOW_WED,
	DOW_THU,
	DOW_FRI,
	DOW_SAT
};

enum
{
	MON_JAN = 0,
	MON_FEB,
	MON_MAR,
	MON_APR,
	MON_MAY,
	MON_JUN,
	MON_JUL,
	MON_AUG,
	MON_SEP,
	MON_OCT,
	MON_NOV,
	MON_DEC
};

struct Date
{
	short int dayOfWeek;
	short int day;
	short int month;
	short int year;
	short int hour;
	short int minute;
	short int second;
};

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

static constexpr const char *ToASCIIEscapeString(char c)
{
	switch (c)
	{
	case '\0':
		return "\\00";
	case '\x01':
		return "\\x01";
	case '\x02':
		return "\\x02";
	case '\x03':
		return "\\x03";
	case '\x04':
		return "\\x04";
	case '\x05':
		return "\\x05";
	case '\x06':
		return "\\x06";
	case '\x07':
		return "\\x07";
	case '\x08':
		return "\\x08";
	case '\x09':
		return "\\x09";
	case '\x0a':
		return "\\x0a";
	case '\x0b':
		return "\\x0b";
	case '\x0c':
		return "\\x0c";
	case '\x0d':
		return "\\x0d";
	case '\x0e':
		return "\\x0e";
	case '\x0f':
		return "\\x0f";
	case '\x10':
		return "\\x10";
	case '\x11':
		return "\\x11";
	case '\x12':
		return "\\x12";
	case '\x13':
		return "\\x13";
	case '\x14':
		return "\\x14";
	case '\x15':
		return "\\x15";
	case '\x16':
		return "\\x16";
	case '\x17':
		return "\\x17";
	case '\x18':
		return "\\x18";
	case '\x19':
		return "\\x19";
	case '\x1a':
		return "\\x1a";
	case '\x1b':
		return "\\x1b";
	case '\x1c':
		return "\\x1c";
	case '\x1d':
		return "\\x1d";
	case '\x1e':
		return "\\x1e";
	case '\x1f':
		return "\\x1f";
	case '\x20':
		return "\\x20";
	case '\x7f':
		return "\\x7f";
	default:
		return nullptr;
	}
}

static inline StringBuilder &AppendHTTPDate(StringBuilder &builder, const Date &date)
{
	switch (date.dayOfWeek)
	{
	case DOW_SUN:
		builder.Append("Sun");
		break;
	case DOW_MON:
		builder.Append("Mon");
		break;
	case DOW_TUE:
		builder.Append("Tue");
		break;
	case DOW_WED:
		builder.Append("Wed");
		break;
	case DOW_THU:
		builder.Append("Thu");
		break;
	case DOW_FRI:
		builder.Append("Fri");
		break;
	case DOW_SAT:
		builder.Append("Sat");
		break;
	}
	builder.Append(", ");

	if (date.day < 10)
		builder.Append('0');
	builder.Append(std::to_string(date.day).c_str());
	builder.Append(' ');

	switch (date.month)
	{
	case MON_JAN:
		builder.Append("Jan");
		break;
	case MON_FEB:
		builder.Append("Feb");
		break;
	case MON_MAR:
		builder.Append("Mar");
		break;
	case MON_APR:
		builder.Append("Apr");
		break;
	case MON_MAY:
		builder.Append("May");
		break;
	case MON_JUN:
		builder.Append("Jun");
		break;
	case MON_JUL:
		builder.Append("Jul");
		break;
	case MON_AUG:
		builder.Append("Aug");
		break;
	case MON_SEP:
		builder.Append("Sep");
		break;
	case MON_OCT:
		builder.Append("Oct");
		break;
	case MON_NOV:
		builder.Append("Nov");
		break;
	case MON_DEC:
		builder.Append("Dec");
		break;
	}
	builder.Append(' ');

	builder.Append(std::to_string(date.year).c_str());
	builder.Append(' ');

	if (date.hour < 10)
		builder.Append('0');
	builder.Append(std::to_string(date.hour));
	builder.Append(':');

	if (date.minute < 10)
		builder.Append('0');
	builder.Append(std::to_string(date.minute));
	builder.Append(':');

	if (date.second < 10)
		builder.Append('0');
	builder.Append(std::to_string(date.second));
	builder.Append(' ');

	builder.Append("GMT");

	return builder;
}
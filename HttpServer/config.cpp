#include "config.h"

#include <stdio.h>
#include "util.h"
#include "string_builder.h"

static constexpr const char HexPrefix[] = "0x";
static constexpr int HexPrefixLength = sizeof(HexPrefix) - 1;

static constexpr const char TrueString[] = "true";

static bool Split(const std::string &full, CaseInsensitiveString *left, std::string *right);

void ConfigFile::Section::AddValue(const CaseInsensitiveString &key, const char *value, int valuelen)
{
	Value cval = {
		value,
		0,
		false
	};

	char *end;
	if (StartsWith(value, valuelen, HexPrefix, HexPrefixLength))
		cval.intValue = strtol(value, &end, 16);
	else
		cval.intValue = strtol(value, &end, 10);

	if (equalsIgnoreCase(value, TrueString))
		cval.boolValue = true;

	m_values[key] = cval;
}

ConfigFile::ConfigFile(const char *file)
{
	FILE *stream;
	fopen_s(&stream, file, "r");
	if (!stream) return;

	fseek(stream, 0, SEEK_END);
	long maxsize = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char *buf = new char[maxsize + 1];

	int actualSize = fread(buf, sizeof(char), maxsize, stream);
	buf[actualSize] = 0;

	fclose(stream);

	std::vector<std::string> lines;
	Tokenize(buf, "\n", &lines);

	delete[] buf;

	CaseInsensitiveString currsec;

	CaseInsensitiveString left;
	std::string right;
	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i] = Trim(lines[i]);
		if (lines[i].length() == 0 || lines[i][0] == ';' || lines[i][0] == '#') continue;

		if (lines[i][0] == '[')
		{
			if (lines[i][lines[i].length() - 1] != ']') continue;

			currsec = lines[i].substr(1, lines[i].length() - 2);
			continue;
		}

		if (currsec.length() > 0)
		{
			if (Split(lines[i], &left, &right))
				m_sections[currsec].AddValue(left, right.c_str(), right.length());
		}
	}
}

bool Split(const std::string &full, CaseInsensitiveString *left, std::string *right)
{
	constexpr int Left = 0, Right = 1;

	left->clear();
	right->clear();

	StringBuilder builder;

	int side = Left;
	bool inquotes = false;
	bool inescape = false;

	for (size_t i = 0; i < full.length(); i++)
	{
		char c = full[i];
		switch (c)
		{
		case '=':
		case ':':
			if (inquotes)
			{
				builder.Append(c);
				continue;
			}
			else
			{
				if (side == Left)
				{
					*left = builder.ToInPlaceString();
					builder.Clear();
					side = Right;
				}
				else
					i = full.length();
			}
			continue;
			break;
		case '\"':
			if (inescape)
			{
				builder.Append('\"');
				inescape = false;
				continue;
			}
			inquotes = !inquotes;
			continue;
			break;
		case '\\':
			if (inescape)
			{
				builder.Append('\\');
				inescape = false;
			}
			else inescape = true;
			continue;
			break;
		case '\'':
			if (inescape)
				inescape = false;
			builder.Append('\'');
			continue;
			break;
		case '0':
			if (inescape)
			{
				builder.Append('\0');
				inescape = false;
				continue;
			}
			break;
		case 'a':
			if (inescape)
			{
				builder.Append('\a');
				inescape = false;
				continue;
			}
			break;
		case 'b':
			if (inescape)
			{
				builder.Append('\b');
				inescape = false;
				continue;
			}
			break;
		case 't':
			if (inescape)
			{
				builder.Append('\t');
				inescape = false;
				continue;
			}
			break;
		case 'r':
			if (inescape)
			{
				builder.Append('\r');
				inescape = false;
				continue;
			}
			break;
		case 'n':
			if (inescape)
			{
				builder.Append('\n');
				inescape = false;
				continue;
			}
			break;
		case ';':
		case '#':
			if (inescape)
			{
				builder.Append(c);
				inescape = false;
				continue;
			}
			goto exit_early;
			break;
		case ' ':
			if (inquotes)
				builder.Append(' ');
			continue;
			break;
		}
		builder.Append(c);
	}
exit_early:
	if (side == Right)
		*right = builder.ToInPlaceString();
	return left->length() > 0 && right->length() > 0;
}

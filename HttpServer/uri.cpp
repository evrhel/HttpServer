#include "uri.h"

#include "util.h"

static constexpr char SchemeSeparator[] = "://";
static constexpr int SchemeSeparatorLength = sizeof SchemeSeparator - 1;

static constexpr char UserInfoSeparator[] = "@";
static constexpr int UserInfoSeparatorLength = sizeof UserInfoSeparator - 1;

static constexpr char PortSeparator[] = ":";
static constexpr int PortSeparatorLength = sizeof PortSeparator - 1;

static constexpr char PathSeparator[] = "/";
static constexpr int PathSeparatorLength = sizeof PathSeparator - 1;

static constexpr char QuerySeparator[] = "?";
static constexpr int QuerySeparatorLength = sizeof QuerySeparator - 1;

static constexpr char QueryDelimeter[] = "&";
static constexpr int QueryDelimeterLength = sizeof QueryDelimeter - 1;

static constexpr char FragmentSeparator[] = "#";
static constexpr int FragmentSeparatorLength = sizeof FragmentSeparator - 1;

bool URI::Parse(const std::string &string)
{
	std::string queries;

	int queryidx = FindFirstOf(string.c_str(), string.length(), QuerySeparator, QuerySeparatorLength);
	int fragidx = FindFirstOf(string.c_str(), string.length(), FragmentSeparator, FragmentSeparatorLength);

	if (queryidx != -1)
	{
		size_t count = fragidx == -1 ? (size_t)-1 : string.length() - queryidx;
		queries = string.substr(queryidx + 1, count);


		m_path = string.substr(0, queryidx);
	}

	if (fragidx != -1)
	{
		m_fragment = string.substr(fragidx + 1);

		if (queryidx == -1)
			m_path = string.substr(0, fragidx);
	}

	if (m_path.length() == 0)
		m_path = string;

	if (queries.length() > 0)
	{
		std::vector<std::string> tokens;
		Tokenize((char *)queries.c_str(), QueryDelimeter, &tokens);

		for (size_t idx = 0; idx < tokens.size(); idx++)
		{
			std::string &query = tokens[idx];
			std::vector<std::string> kv;
			Tokenize((char *)query.c_str(), "=", &kv);
			if (kv.size() == 2)
				m_queries[kv[0]] = kv[1];
		}
	}

	return true;
}
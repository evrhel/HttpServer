#pragma once

#include <string>
#include <unordered_map>
#include <strutil/cpp_string_util.h>

using namespace strutil;

class URI
{
private:
	std::string m_scheme;
	std::string m_userinfo;
	std::string m_host;
	std::string m_port;
	std::string m_path;
	std::unordered_map<CaseInsensitiveString, std::string> m_queries;
	std::string m_fragment;
public:
	inline URI() { }
	inline URI(const std::string &uristr) { Parse(uristr); }

	bool Parse(const std::string &string);

	constexpr const std::string &GetScheme() const
	{
		return m_scheme;
	}

	constexpr const std::string &GetUserInfo() const
	{
		return m_userinfo;
	}

	constexpr const std::string &GetPort() const
	{
		return m_port;
	}

	constexpr const std::string &GetPath() const
	{
		return m_path;
	}

	constexpr const std::unordered_map<CaseInsensitiveString, std::string> &GetQueries() const
	{
		return m_queries;
	}

	inline const std::string *FindQuery(const CaseInsensitiveString &attribute) const
	{
		auto it = m_queries.find(attribute);
		return it == m_queries.end() ? nullptr : &it->second;
	}

	constexpr const std::string &GetFragment() const
	{
		return m_fragment;
	}
};
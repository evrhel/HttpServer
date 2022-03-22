#pragma once

#include <string>
#include <stdlib.h>

#include "util.h"

enum
{
	SECURE_UNSPEC = -1,

	SECURE_FALSE = 0,
	SECURE_TRUE = 1
};

enum
{
	HTTPONLY_UNSPEC = -1,

	HTTPONLY_FALSE = 0,
	HTTPONLY_TRUE
};

enum
{
	SAMESITE_UNSEPC = -1,

	SAMESITE_STRICT = 0,
	SAMESITE_LAX,
	SAMESITE_NONE
};

class HTTPCookie
{
private:
	std::string m_name;
	std::string m_value;
	Date m_date;
	long long int m_maxage;
	std::string m_domain;
	std::string m_path;
	int m_secure;
	int m_httpOnly;
	int m_samesite;
public:
	inline HTTPCookie() :
		m_name(), m_value(), m_date(), m_maxage(-1), m_domain(), m_path(),
		m_secure(SECURE_UNSPEC), m_httpOnly(HTTPONLY_UNSPEC), m_samesite(SAMESITE_STRICT)
	{
		memset(&m_date, 0xff, sizeof(m_date));
	}

	explicit inline HTTPCookie(const std::string &name, const std::string &value = std::string()) :
		m_name(name), m_value(value), m_date(), m_maxage(-1), m_domain(), m_path(),
		m_secure(SECURE_UNSPEC), m_httpOnly(HTTPONLY_UNSPEC), m_samesite(SAMESITE_STRICT)
	{
		memset(&m_date, 0xff, sizeof(m_date));
	}

	inline HTTPCookie(const HTTPCookie &other) :
		m_name(other.m_name), m_value(other.m_value), m_date(other.m_date), m_maxage(other.m_maxage),
		m_domain(other.m_domain), m_path(other.m_path), m_secure(other.m_secure), m_httpOnly(other.m_httpOnly),
		m_samesite(other.m_samesite) { }

	inline HTTPCookie &operator=(const HTTPCookie &other)
	{
		if (this != &other)
		{
			m_name = other.m_name;
			m_value = other.m_value;
			m_date = other.m_date;
			m_maxage = other.m_maxage;
			m_domain = other.m_domain;
			m_path = other.m_path;
			m_secure = other.m_secure;
			m_httpOnly = other.m_httpOnly;
			m_samesite = other.m_samesite;
		}
		return *this;
	}

	inline HTTPCookie &operator=(HTTPCookie &&other) noexcept
	{
		if (this != &other)
		{
			m_name = std::move(other.m_name);
			m_value = std::move(other.m_value);
			m_date = other.m_date;
			m_maxage = other.m_maxage;
			m_domain = std::move(other.m_domain);
			m_path = std::move(other.m_path);
			m_secure = other.m_secure;
			m_httpOnly = other.m_httpOnly;
			m_samesite = other.m_samesite;
		}
		return *this;
	}

	inline HTTPCookie &SetName(const std::string &name)
	{
		m_name = name;
		return *this;
	}

	inline HTTPCookie &SetValue(const std::string &value)
	{
		m_value = value;
		return *this;
	}

	constexpr HTTPCookie &SetExpiration(const Date &date)
	{
		m_date = date;
		return *this;
	}

	inline HTTPCookie &RemoveExpiration()
	{
		memset(&m_date, 0xff, sizeof(m_date));
		return *this;
	}

	inline HTTPCookie &SetMaxAge(long long int seconds)
	{
		m_maxage = seconds;
		return *this;
	}

	constexpr HTTPCookie &RemoveMaxAge()
	{
		m_maxage = -1;
		return *this;
	}

	inline HTTPCookie &SetDomain(const std::string &domain)
	{
		m_domain = domain;
		return *this;
	}

	inline HTTPCookie &SetPath(const std::string &path)
	{
		m_path = path;
		return *this;
	}

	constexpr HTTPCookie &SetSecure(int state)
	{
		m_secure = state;
		return *this;
	}

	constexpr HTTPCookie &SetHTTPOnly(int state)
	{
		m_httpOnly = state;
		return *this;
	}

	constexpr HTTPCookie &SetSameSite(int state)
	{
		m_samesite = state;
		return *this;
	}

	constexpr const std::string &GetName() const
	{
		return m_name;
	}

	constexpr const std::string &GetValue() const
	{
		return m_value;
	}

	constexpr const Date &GetExpiration() const
	{
		return m_date;
	}

	constexpr long long int GetMaxAge() const
	{
		return m_maxage;
	}

	constexpr const std::string &GetDomain() const
	{
		return m_domain;
	}

	constexpr const std::string &GetPath() const
	{
		return m_path;
	}

	constexpr int IsSecure() const
	{
		return m_secure;
	}

	constexpr int IsHTTPOnly() const
	{
		return m_httpOnly;
	}

	constexpr int GetSameSite() const
	{
		return m_samesite;
	}

	StringBuilder &AppendToBuilder(StringBuilder &builder) const;
};
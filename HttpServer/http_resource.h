#pragma once

#include <string>

#include "common.h"

std::string GetExtension(const std::string &path);
void ResolveContentType(const char *ext, std::string &out);

class HTTPResource
{
private:
	std::string m_name;
	std::string m_location;
	char *m_data;
	size_t m_len;

	std::string m_contentType;

	size_t m_refs;

	HANDLE m_mutex;

	bool LoadResource();
public:
	HTTPResource(const std::string &name, const std::string &location);
	~HTTPResource();

	bool Request();
	void Done();

	constexpr const std::string &GetName() const
	{
		return m_name;
	}

	constexpr const std::string &GetLocation() const
	{
		return m_location;
	}

	constexpr const char *GetData() const
	{
		return m_data;
	}

	constexpr size_t GetDataLength() const
	{
		return m_len;
	}
	
	inline const char *GetContentType() const
	{
		return m_contentType.c_str();
	}

	constexpr size_t GetDRAMUsage() const
	{
		return m_len;
	}

	constexpr size_t GetMemoryMappedSize() const
	{
		return 0;
	}
};
#include "http_resource.h"

#include <stdio.h>
#include <strutil/cpp_string_util.h>

using namespace strutil;

std::string GetExtension(const std::string &path)
{
	size_t sepidx = path.find_last_of('/');
	size_t extidx = path.find_last_of('.');
	if (extidx != -1 && extidx > sepidx)
	{
		const char *ext = path.c_str() + extidx + 1;
		return ext;
	}
	return std::string();
}

void ResolveContentType(const char *ext, std::string &out)
{
	if (equalsIgnoreCase(ext, "mp3"))
		out = "audio/mp3";
	else if (equalsIgnoreCase(ext, "wma"))
		out = "audio/x-ms-wma";
	else if (equalsIgnoreCase(ext, "wav"))
		out = "audio/x-wav";
	else if (equalsIgnoreCase(ext, "gif"))
		out = "image/gif";
	else if (equalsIgnoreCase(ext, "jpg") || equalsIgnoreCase(ext, "jpeg"))
		out = "image/jpeg";
	else if (equalsIgnoreCase(ext, "png"))
		out = "image/jpeg";
	else if (equalsIgnoreCase(ext, "tiff"))
		out = "image/jpeg";
	else if (equalsIgnoreCase(ext, "ico"))
		out = "image/x-icon";
	else if (equalsIgnoreCase(ext, "css"))
		out = "text/css";
	else if (equalsIgnoreCase(ext, "csv"))
		out = "text/csv";
	else if (equalsIgnoreCase(ext, "html") || equalsIgnoreCase(ext, "htm"))
		out = "text/html";
	else if (equalsIgnoreCase(ext, "js"))
		out = "text/javascript";
	else if (equalsIgnoreCase(ext, "txt"))
		out = "text/plain";
	else if (equalsIgnoreCase(ext, "xml"))
		out = "text/xml";
	else if (equalsIgnoreCase(ext, "mpeg"))
		out = "video/mpeg";
	else if (equalsIgnoreCase(ext, "mp4"))
		out = "video/mp4";
}

bool HTTPResource::LoadResource()
{
	FILE *stream;
	fopen_s(&stream, m_location.c_str(), "rb");
	if (!stream) return false;

	fseek(stream, 0, SEEK_END);
	m_len = (size_t)_ftelli64(stream);
	fseek(stream, 0, SEEK_SET);

	m_data = new char[m_len];
	fread_s(m_data, m_len, 1, m_len, stream);

	fclose(stream);

	return true;
}

HTTPResource::HTTPResource(const std::string &name, const std::string &location) :
	m_name(name), m_location(location), m_data(nullptr), m_len(0), m_refs(0),
	m_mutex(NULL)
{
	size_t sepidx = name.find_last_of('/');
	size_t extidx = name.find_last_of('.');
	if (extidx != -1 && extidx > sepidx)
	{
		const char *ext = name.c_str() + extidx + 1;
		ResolveContentType(ext, m_contentType);
	}
	else m_contentType = "text/plain";
	m_mutex = CreateMutexA(NULL, FALSE, NULL);
}

HTTPResource::~HTTPResource()
{
	CloseHandle(m_mutex);
	m_mutex = NULL;
}

bool HTTPResource::Request()
{
	DWORD dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		if (!LoadResource())
		{
			ReleaseMutex(m_mutex);
			return false;
		}
		m_refs++;
		ReleaseMutex(m_mutex);
		return true;
		break;
	case WAIT_ABANDONED:
		ReleaseMutex(m_mutex);
		return false;
		break;
	}

	ReleaseMutex(m_mutex);
	return false;
}

void HTTPResource::Done()
{
	DWORD dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		if (m_refs > 0)
			m_refs--;
		if (m_refs == 0)
		{
			delete[] m_data;
			m_data = nullptr;
			m_len = 0;
		}
		break;
	}
	ReleaseMutex(m_mutex);
}

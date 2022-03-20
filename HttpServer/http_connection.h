#pragma once

#include <unordered_map>
#include <strutil/cpp_string_util.h>

#include "string_builder.h"
#include "client_connection.h"
#include "uri.h"

using namespace strutil;

class HTTPRequest;
class HTTPResponse;
class HTTPConnection;

using HTTPRequestHandlerFunc = HTTPResponse * (*)(const HTTPRequest *request);

enum
{
	METHOD_NONE = -1,
	METHOD_CONNECT = 0,
	METHOD_DELETE = 1,
	METHOD_GET = 2,
	METHOD_HEAD = 3,
	METHOD_OPTIONS = 4,
	METHOD_POST = 5,
	METHOD_PUT = 6,
	METHOD_TRACE = 7,

	METHOD_COUNT = METHOD_TRACE + 1
};

enum
{
	RESP_OK = 200,

	RESP_BAD_REQUEST = 400,
	RESP_UNAUTHORIZED = 401,
	RESP_FORBIDDEN = 403,
	RESP_NOT_FOUND = 404,
	RESP_METHOD_NOT_ALLOWED = 405
};

const char *GetMethodString(int method);
int GetMethodFromString(const char *str);

class HTTPRequest
{
private:
	friend class HTTPConnection;
	
	int m_method;
	URI m_uri;
	std::unordered_map<CaseInsensitiveString, CaseInsensitiveString> m_queries;
	std::unordered_map<CaseInsensitiveString, std::string> m_headers;
	char *m_content;
	int m_contentlen;
	HTTPConnection *m_source;
public:
	inline HTTPRequest() :
		m_method(METHOD_NONE), m_uri(), m_headers(), m_content(nullptr), m_contentlen(0), m_source(nullptr) { }
	inline ~HTTPRequest()
	{
		if (m_content)
			delete[] m_content;
	}

	HTTPRequest(const HTTPRequest &) = delete;

	constexpr int GetMethod() const
	{
		return m_method;
	}

	constexpr const URI &GetURI() const
	{
		return m_uri;
	}
	
	inline const CaseInsensitiveString *GetQuery(const CaseInsensitiveString &name) const
	{
		auto it = m_queries.find(name);
		return it == m_queries.end() ? nullptr : &it->second;
	}

	inline const std::string *GetHeader(const CaseInsensitiveString &name) const
	{
		auto it = m_headers.find(name);
		return it == m_headers.end() ? nullptr : &it->second;
	}

	constexpr const char *GetContent() const
	{
		return m_content;
	}

	constexpr int GetContentLength() const
	{
		return m_contentlen;
	}

	constexpr HTTPConnection *GetSource() const
	{
		return m_source;
	}
};

class HTTPResponse
{
private:
	int m_code;
	std::string m_reason;
	std::unordered_map<CaseInsensitiveString, std::string> m_headers;
	StringBuilder m_content;
public:
	inline HTTPResponse() :
		m_code(0), m_reason(), m_headers(), m_content() { }
	inline HTTPResponse(size_t expectedcontentlen) :
		m_code(0), m_reason(), m_headers(), m_content(expectedcontentlen) { }

	inline ~HTTPResponse() { }
	
	HTTPResponse(const HTTPResponse &) = delete;

	constexpr void SetCode(int code)
	{
		m_code = code;
	}

	constexpr void SetReason(const char *reason)
	{
		if (reason && *reason) m_reason = reason;
	}

	inline void AddHeader(const CaseInsensitiveString &name, const std::string &value)
	{
		if (name.length() > 0 && value.length() > 0)
			m_headers[name] = value;
	}

	inline void SetContentType(const std::string &type)
	{
		static CaseInsensitiveString CONTENT_TYPE_KEY("Content-Type");
		AddHeader(CONTENT_TYPE_KEY, type);
	}

	inline void AppendContent(const char *data, size_t len)
	{
		m_content.Append(data, len);
	}

	inline const HTTPResponse *Finalize()
	{
		static CaseInsensitiveString CONTENT_LENGTH_KEY("Content-Length");
		static CaseInsensitiveString SERVER_KEY("Server");
		
		AddHeader(CONTENT_LENGTH_KEY, std::to_string(m_content.Size()));
		AddHeader(SERVER_KEY, "HttpServer/1.0");
		return this;
	}

	constexpr int GetCode() const
	{
		return m_code;
	}

	inline const char *GetReason() const
	{
		return m_reason.c_str();
	}

	constexpr const std::unordered_map<CaseInsensitiveString, std::string> &GetHeaders() const
	{
		return m_headers;
	}

	inline const char *GetContent() const
	{
		return m_content.GetElements();
	}

	inline size_t GetContentLength() const
	{
		return m_content.Size();
	}
};

class HTTPConnection
{
private:
	ClientConnection *m_connection;
	void *m_httpServer;

	StringBuilder m_buffer;
public:
	inline HTTPConnection(void *httpServer) : m_connection(nullptr), m_httpServer(httpServer) { }
	~HTTPConnection();

	bool Bind(ClientConnection *connection);
	void Close();

	constexpr const ClientConnection *GetConnection() const
	{
		return m_connection;
	}

	constexpr bool IsOpen() const
	{
		return m_connection != nullptr;
	}

	constexpr void *GetHTTPServer() const
	{
		return m_httpServer;
	}

	HTTPRequest *GetNextRequest();
	int SendResponse(const HTTPResponse *response);
};
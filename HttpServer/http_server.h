#pragma once

#include <unordered_map>
#include <strutil/cpp_string_util.h>

#include "common.h"
#include "server.h"
#include "http_resource.h"
#include "http_connection.h"

using namespace strutil;

class HTTPServer
{
private:
	static DWORD HTTPServerWorker(__in HTTPServer *httpServer);
private:
	Server *m_server;
	HANDLE m_handle;
	HANDLE m_rsrcMutex;

	std::string m_resourcedir;
	std::unordered_map<CaseInsensitiveString, HTTPResource *> m_resources;
	std::unordered_map<CaseInsensitiveString, CaseInsensitiveString> m_resourceProxies;

	HTTPRequestHandlerFunc m_handleFuncs[METHOD_COUNT];

	void LoadResources(const std::string &resourcedir);
public:
	HTTPServer(const std::string &resourcedir);
	~HTTPServer();

	constexpr void SetRequestHandler(int request, HTTPRequestHandlerFunc func)
	{
		if (request >= 0 && request < METHOD_COUNT)
			m_handleFuncs[request] = func;
	}

	constexpr HTTPRequestHandlerFunc GetRequestHandlerFunc(int method) const
	{
		if (method < 0 || method >= METHOD_COUNT)
			return nullptr;
		return m_handleFuncs[method];
	}

	void CreateResourceProxy(const CaseInsensitiveString &from, const CaseInsensitiveString &to);

	bool Bind(Server *server);
	void Close();
	void ForceClose();

	bool DispatchServer();
	bool WaitUntilFinish();

	constexpr const std::string &GetResourceDirectory() const
	{
		return m_resourcedir;
	}

	bool ReloadResources();

	HTTPResource *FindHTTPResource(const CaseInsensitiveString &location) const;

	const std::unordered_map<CaseInsensitiveString, HTTPResource *> &GetResources() const;

	const std::unordered_map<CaseInsensitiveString, CaseInsensitiveString> &GetResourceProxies() const;

	void GenerateAllowHeader(HTTPResponse *dest) const;
};
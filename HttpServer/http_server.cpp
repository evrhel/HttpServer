#include "http_server.h"

#include <assert.h>

struct HTTPConnectionWorkerInfo
{
	HTTPConnection *connection;
	HTTPServer *server;

	HTTPConnectionWorkerInfo(HTTPConnection *connection, HTTPServer *server) :
		connection(connection), server(server) { }
};

static DWORD HTTPConnectionWorker(__in HTTPConnectionWorkerInfo *info);

static HTTPResponse *HandleUnsupportedRequest(const HTTPRequest *request);

static void FindFiles(const char *root, std::vector<std::string> &paths);

DWORD HTTPServer::HTTPServerWorker(__in HTTPServer *httpServer)
{
	Server *server = httpServer->m_server;
	ClientConnection *connection;
	HTTPConnection *con;
	while (true)
	{
		connection = server->Accept();
		if (!connection)
			break;

		//printf("Client connected from: %s:%hu\n", connection->GetRemoteAddress(), connection->GetPort());

		con = new HTTPConnection(httpServer);
		if (!con->Bind(connection))
		{
			delete con;
			break;
		}

		HTTPConnectionWorkerInfo *info = new HTTPConnectionWorkerInfo(con, httpServer);
		HANDLE connectionWorker = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&HTTPConnectionWorker, info, 0, NULL);
		if (!connectionWorker)
		{
			delete info;
			delete connectionWorker;
			continue;
		}

		// detach the thread
		CloseHandle(connectionWorker);
	}
	return 0;
}

void HTTPServer::LoadResources(const std::string &resourcedir)
{
	std::vector<std::string> files;
	FindFiles(resourcedir.c_str(), files);

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string normalized(files[i].c_str() + resourcedir.length());
		char *tochange = (char *)normalized.c_str();
		while (*tochange)
		{
			if (*tochange == '\\') *tochange = '/';
			tochange++;
		}

		m_resources[normalized] = new HTTPResource(normalized, files[i]);
		printf("Registered resource: %s\n", normalized.c_str());
	}

	printf("Registered %zu resources!\n", files.size());
}

HTTPServer::HTTPServer(const std::string &resourcedir) :
	m_server(nullptr), m_handle(NULL), m_resources()
{
	memset(m_handleFuncs, 0, sizeof(m_handleFuncs));
	LoadResources(resourcedir);
}

HTTPServer::~HTTPServer()
{
	Close();
}

void HTTPServer::CreateResourceProxy(const CaseInsensitiveString &from, const CaseInsensitiveString &to)
{
	m_resourceProxies[from] = to;
	printf("Created proxy from resource %s to %s\n", from.cstr(), to.cstr());
}

bool HTTPServer::Bind(Server *server)
{
	if (m_server || !server) return false;
	m_server = server;
}

void HTTPServer::Close()
{
	if (m_server)
	{
		m_server->Close();
		if (m_handle)
		{
			WaitForSingleObject(m_handle, INFINITE);  // must wait for the thread to finish before freeing
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
		m_server = nullptr;
	}
}

void HTTPServer::ForceClose()
{
	if (m_server)
	{
		if (m_handle)
		{
			CloseHandle(m_handle);
			m_handle = nullptr;
		}

		m_server = nullptr;
	}
}

bool HTTPServer::DispatchServer()
{
	if (m_handle) return false;
	m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&HTTPServerWorker, this, 0, NULL);
	return m_handle;
}

bool HTTPServer::WaitUntilFinish()
{
	if (m_handle)
	{
		WaitForSingleObject(m_handle, INFINITE);
		CloseHandle(m_handle);
		m_handle = nullptr;
		return true;
	}
	return false;
}

HTTPResource *HTTPServer::FindHTTPResource(const CaseInsensitiveString &location) const
{
	const CaseInsensitiveString *actual;
	auto proxit = m_resourceProxies.find(location);
	if (proxit != m_resourceProxies.end())
		actual = &proxit->second;
	else
		actual = &location;

	auto it = m_resources.find(*actual);
	return it == m_resources.end() ? nullptr : it->second;
}

DWORD HTTPConnectionWorker(__in HTTPConnectionWorkerInfo *info)
{
	static CaseInsensitiveString CONNECTION_HEADER("Connection");

	HTTPConnection *connection = info->connection;
	HTTPServer *server = info->server;

	delete info;

	do
	{
		HTTPRequest *req = connection->GetNextRequest();
		if (!req) break;

		const std::string *conheader = req->GetHeader(CONNECTION_HEADER);
		if (conheader && equalsIgnoreCase(*conheader, "close"))
		{
			delete req;
			break;
		}

		HTTPRequestHandlerFunc fun = server->GetRequestHandlerFunc(req->GetMethod());
		if (!fun) fun = &HandleUnsupportedRequest;

		HTTPResponse *response = fun(req);
		if (!response)
		{
			assert(fun != &HandleUnsupportedRequest);
			response = HandleUnsupportedRequest(req);
			if (!response)
			{
				delete req;
				break;
			}
		}

		delete req;

		if (connection->SendResponse(response->Finalize()) <= 0)
		{
			delete response;
			break;
		}

		delete response;
	} while (true);

	delete connection;
	return 0;
}

HTTPResponse *HandleUnsupportedRequest(const HTTPRequest *request)
{
	HTTPResponse *response = new HTTPResponse();

	response->SetCode(RESP_METHOD_NOT_ALLOWED);
	response->SetReason(GetMethodString(request->GetMethod()));

	StringBuilder allowed;

	HTTPServer *server = (HTTPServer *)request->GetSource()->GetHTTPServer();
	for (int method = 0; method < METHOD_COUNT; method++)
	{
		if (server->GetRequestHandlerFunc(method))
		{
			if (allowed.Size() > 0)
				allowed.Append(", ");
			allowed.Append(GetMethodString(method));
		}
	}

	response->AddHeader("Allow", allowed.ToInPlaceString());

	static constexpr char Message[] = "405 Method Not Allowed";

	response->SetContentType("text/plain");
	response->AppendContent(Message, sizeof(Message) - 1);

	return response;
}

void FindFiles(const char *root, std::vector<std::string> &paths)
{
	WIN32_FIND_DATAA ffd;
	CHAR szNewPath[MAX_PATH];

	sprintf_s(szNewPath, "%s\\*", root);

	HANDLE hFind = FindFirstFileA(szNewPath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("ERROR> Finding resources in directory %s\n", root);
		return;
	}

	do
	{
		if (!equalsIgnoreCase(ffd.cFileName, root) &&
			!equalsIgnoreCase(ffd.cFileName, ".") &&
			!equalsIgnoreCase(ffd.cFileName, "..")
			)
		{
			sprintf_s(szNewPath, "%s\\%s", root, ffd.cFileName);
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				FindFiles(szNewPath, paths);
			else
				paths.push_back(szNewPath);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		printf("ERROR> Find failure in directory %s\n", root);

	FindClose(hFind);
}

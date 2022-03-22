#include "server.h"

Server::Server() : m_server(INVALID_SOCKET), m_mutex(NULL)
{
	m_mutex = CreateMutexA(0, FALSE, NULL);
}

Server::~Server()
{
	Close();
	CloseHandle(m_mutex);
	m_mutex = NULL;
}

bool Server::Open(USHORT port, int family)
{
	DWORD dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_ABANDONED:
		return false;
		break;
	}

	if (m_server != INVALID_SOCKET)
	{
		ReleaseMutex(m_mutex);
		return false;
	}

	SOCKET server = socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		ReleaseMutex(m_mutex);
		return false;
	}

	/*SOCKET_SECURITY_SETTINGS sssSetings = {
		SOCKET_SECURITY_PROTOCOL_DEFAULT,
		SOCKET_SETTINGS_GUARANTEE_ENCRYPTION
	};

	int res = WSASetSocketSecurity(server, &sssSetings, sizeof(sssSetings), NULL, NULL);
	if (res == SOCKET_ERROR)
	{
		ReleaseMutex(m_mutex);
		return false;
	}*/

	struct sockaddr_storage storage;
	int addrlen;

	if (family == AF_INET)
	{
		struct sockaddr_in *addr = (struct sockaddr_in *)&storage;
		addrlen = sizeof(*addr);

		memset(addr, 0, sizeof(*addr));
		addr->sin_family = AF_INET;
		addr->sin_port = htons(port);

		if (inet_pton(AF_INET, "127.0.0.1", &addr->sin_addr) != 1)
		{
			closesocket(server);

			ReleaseMutex(m_mutex);
			return false;
		}
	}
	else if (family == AF_INET6)
	{
		struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&storage;
		addrlen = sizeof(*addr);

		memset(addr, 0, addrlen);
		addr->sin6_family = AF_INET6;
		addr->sin6_flowinfo = 0;
		addr->sin6_port = htons(port);
		//addr.sin6_addr = in6addr_any;

		//service.sin6_family = AF_INET6;
		//service.sin_addr.s_addr;

		if (inet_pton(AF_INET6, "::1", &addr->sin6_addr) != 1)
		{
			closesocket(server);

			ReleaseMutex(m_mutex);
			return false;
		}
		//if (inet_pt)
		//service.sin_addr.s_addr = inet_addr("127.0.0.1");
		//service.sin6_port = htons(port);
	}


	if (bind(server, (struct sockaddr *)&storage, addrlen) == SOCKET_ERROR)
	{
		closesocket(server);

		ReleaseMutex(m_mutex);
		return false;
	}

	if (listen(server, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(server);

		ReleaseMutex(m_mutex);
		return false;
	}

	m_server = server;

	ReleaseMutex(m_mutex);
	return true;
}

void Server::Close()
{
	DWORD dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_ABANDONED:
		break;
	}

	if (m_server != INVALID_SOCKET)
	{
		closesocket(m_server);
		m_server = INVALID_SOCKET;
	}

	ReleaseMutex(m_mutex);
}

ClientConnection *Server::Accept()
{
	if (m_server == INVALID_SOCKET)
		return nullptr;

	//struct sockaddr_in6 addr;
	//int addrlen = sizeof(addr);

	struct sockaddr_storage storage;
	int addrlen = sizeof(storage);

	SOCKET clsocket = accept(m_server, (sockaddr *)&storage, &addrlen);
	if (clsocket == INVALID_SOCKET) return nullptr;

	ClientConnection *con = new ClientConnection();
	con->Bind(clsocket, *(sockaddr *)&storage);
	return con;
}

#pragma once

#include "client_connection.h"

class Server
{
private:
	SOCKET m_server;
	HANDLE m_mutex;
public:
	Server();
	~Server();

	bool Open(USHORT port, int family);
	void Close();

	ClientConnection *Accept();
};
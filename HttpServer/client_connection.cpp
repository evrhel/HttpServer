#include "client_connection.h"

#include <stdio.h>

ClientConnection::ClientConnection() :
	m_client(INVALID_SOCKET), m_family(0), m_port(0)
{
	memset(&m_addr, 0, sizeof(m_addr));
	memset(m_addrstr, 0, sizeof(m_addrstr));
}

ClientConnection::~ClientConnection()
{
	Close();
}

bool ClientConnection::Bind(SOCKET client, const struct sockaddr &addr)
{
	if (m_client != INVALID_SOCKET) return false;
	m_client = client;
	m_addr = addr;

	if (addr.sa_family == AF_INET)
	{
		const struct sockaddr_in *sinaddr = (const struct sockaddr_in *)&addr;
		
		const struct in_addr *inaddr = &sinaddr->sin_addr;


		sprintf_s(m_addrstr, "%hhu.%hhu.%hhu.%hhu",
			inaddr->S_un.S_un_b.s_b1,
			inaddr->S_un.S_un_b.s_b2,
			inaddr->S_un.S_un_b.s_b3,
			inaddr->S_un.S_un_b.s_b4
		);


		m_port = sinaddr->sin_port;
	}
	else if (addr.sa_family == AF_INET6)
	{
		const struct sockaddr_in6 *sinaddr = (const struct sockaddr_in6 *)&addr;

		const struct in6_addr *inaddr = &sinaddr->sin6_addr;

		sprintf_s(m_addrstr, "%hx::%hx::%hx::%hx::%hx::%hx::%hx::%hx",
			inaddr->u.Word[0],
			inaddr->u.Word[1],
			inaddr->u.Word[2],
			inaddr->u.Word[3],
			inaddr->u.Word[4],
			inaddr->u.Word[5],
			inaddr->u.Word[6],
			inaddr->u.Word[7]
		);

		m_port = sinaddr->sin6_port;

	/*	const struct in_addr *inaddr = &sinaddr->sin_addr;


		sprintf_s(m_addrstr, "%hhu.%hhu.%hhu.%hhu",
			inaddr->S_un.S_un_b.s_b1,
			inaddr->S_un.S_un_b.s_b2,
			inaddr->S_un.S_un_b.s_b3,
			inaddr->S_un.S_un_b.s_b4
		);
		*/


		//m_port = sinaddr->sin_port;
	}

	return true;
}

void ClientConnection::Close()
{
	if (m_client != INVALID_SOCKET)
	{
		closesocket(m_client);

		m_client = INVALID_SOCKET;
		memset(&m_addr, 0, sizeof(m_addr));

		m_family = 0;
		memset(m_addrstr, 0, sizeof(m_addrstr));
		m_port = 0;
	}
}

int ClientConnection::ReadBytes(char *dest, int len)
{
	int res = recv(m_client, dest, len, 0);
	if (res <= 0) Close();
	return res;
}

int ClientConnection::WriteBytes(const char *src, int len)
{
	int res = send(m_client, src, len, 0);
	if (res < 0) Close();
	return res;
}

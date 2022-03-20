#pragma once

#include "common.h"

#include <WS2tcpip.h>

class ClientConnection
{
private:
	SOCKET m_client;
	sockaddr m_addr;

	ADDRESS_FAMILY m_family;
	char m_addrstr[INET6_ADDRSTRLEN + 1];  // max ipv6 length
	unsigned short m_port;
public:
	ClientConnection();
	~ClientConnection();

	bool Bind(SOCKET client, const struct sockaddr &addr);
	void Close();

	constexpr ADDRESS_FAMILY GetAddressFamily() const
	{
		return m_family;
	}

	constexpr const char *GetRemoteAddress() const
	{
		return m_addrstr;
	}

	constexpr unsigned short GetPort() const
	{
		return m_port;
	}
	
	int ReadBytes(char *dest, int len);
	int WriteBytes(const char *src, int len);
};
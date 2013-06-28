// SocketClient.cpp Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#include "SocketClient.h"
#include <QtGlobal>

// some work will be needed to port this to unix-y type platforms
// you will need to lookup the exact header includes for a start
// may need to #define INVALID_SOCKET -1 as well
#ifdef Q_OS_WIN32
#include <winsock.h>
#endif

SocketClient::SocketClient()
{
#ifdef Q_OS_WIN32
    WORD version = 0x0202;
	WSADATA dllinfo;
	int rc = WSAStartup(version, &dllinfo);
    Q_ASSERT(rc == 0);
#endif

	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    Q_ASSERT(m_socket != INVALID_SOCKET);
}


SocketClient::~SocketClient()
{
	close();

#ifdef Q_OS_WIN32
    WSACleanup();
#endif
}

bool 
SocketClient::connect(const std::string &hostname, const unsigned long port)
{
	bool rc=false;
	struct hostent * dnsdata = gethostbyname(hostname.c_str());
	if (dnsdata) {
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons((unsigned short)port);
		memcpy(&(addr.sin_addr),dnsdata->h_addr_list[0],dnsdata->h_length);
		rc = (::connect(m_socket,(sockaddr*)&addr,sizeof(addr)) == 0);
	}
	return rc;
}

bool SocketClient::is_connected()
{
	bool rc=false;
	if (m_socket != INVALID_SOCKET) {
		fd_set writeset;
		FD_ZERO(&writeset);
		FD_SET(m_socket,&writeset);
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		long result = select(1,NULL,&writeset,NULL,&timeout);
		rc = (result > 0);
	}
	return rc;
}

bool SocketClient::read(void *buf, unsigned short &len)
{
	bool rc=false;
	if (m_socket != INVALID_SOCKET) {
		int rlen = recv(m_socket,(char *)buf,len,0);
		rc = (rlen != SOCKET_ERROR && rlen > 0);
		if (rc) {
			len = rlen;
		}
	}
	return rc;
}

bool SocketClient::write(const std::string &data)
{
	bool rc=false;
	if (m_socket != INVALID_SOCKET) {
		const char *p = data.c_str();
		const char *e = data.c_str() + data.length();
		int wrlen;
		do {
			wrlen = send(m_socket,p,e-p,0);
			rc = (wrlen != SOCKET_ERROR);
			if (rc) {
				p += wrlen;
			}
		} while (wrlen != SOCKET_ERROR && p < e);
	}
	return rc;
}

void SocketClient::close()
{
	if (m_socket != INVALID_SOCKET) {
		shutdown(m_socket,2);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

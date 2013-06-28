// SocketClient.h Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#ifndef _SOCKETCLIENT_H_
#define _SOCKETCLIENT_H_

#include <string>

// Why aren't we using QTcpSocket? Because it doesn't work, that's why.
// Mainly using Qt for the GUI and some of the container classes
class SocketClient
{
public:
	SocketClient();
	virtual ~SocketClient();

	bool connect(const std::string &hostname, const unsigned long port);
	bool is_connected();
	bool read(void *buf, unsigned short &len);
	bool write(const std::string &data);
	void close();

private:
	// do not impl these two
	SocketClient(const SocketClient &src);
	SocketClient & operator=(const SocketClient & src);

	unsigned long m_socket;
};

#endif // _SOCKETCLIENT_H_

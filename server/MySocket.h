#ifndef SERVER_MySOCKET_H_

#define SERVER_MySOCKET_H_

#include "util.h"

class MySocket
{
public:
	MySocket(int family, const char* ip, int port);
	~MySocket(void);
	int Create();
	int Listen();
	int Close();
	SOCKET GetSocket();
	int GetLastErrorCode();

private:
	SOCKET _socket;
	int _family;
	const char* _ip;
	int _port;
};

#endif
#ifndef SERVER_ACCEPTSOCKET_H_

#define SERVER_ACCEPTSOCKET_H_

#include "util.h"
#include "MySocket.h"

class AcceptSocket
{
public:
	AcceptSocket(MySocket* socket);
	~AcceptSocket(void);
	int Create();
	int Close();
	int Send(const char* sendbuf);
	int Receive(char* recvbuf, int buf_len);
	int GetLastErrorCode();

private:
	MySocket* _socket;
	SOCKET _accept_socket;
};

#endif
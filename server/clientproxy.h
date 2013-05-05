#ifndef SERVER_CLIENT_H_

#define SERVER_CLIENT_H_

#include "util.h"
#include "acceptSocket.h"

class ClientProxy
{
public:
	ClientProxy(void);
	~ClientProxy(void);
	int create(AcceptSocket* socket, void* server);
	int stop();
	int sendToClient(string sendbuf);
	friend DWORD threadProcedure(ClientProxy* client);
	int getId();
	void setId(int id);

private:
	AcceptSocket* _socket;
	void* _server;
	int _id;
};

#endif


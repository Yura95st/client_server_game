#include "clientproxy.h"
#include "server.h"

#define DEFAULT_BUFLEN 512

ClientProxy::ClientProxy(void)
{
}


ClientProxy::~ClientProxy(void)
{
}

int ClientProxy::create(AcceptSocket* socket, void* srv)
{
	this->_socket = socket;
	this->_server = (Server*)srv;

	DWORD dwThreadId;

	if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&threadProcedure, (LPVOID) this, 0, &dwThreadId) == NULL)
	{
		//printf("Error Creating Thread#: %d\n", Thread_num);
		return 1;
	}

	//Sleep(1000);
	return 0;
}

DWORD threadProcedure(ClientProxy* client)
{
	int iResult;
    char recvbuf[DEFAULT_BUFLEN] = "";
	int id = client->getId();
	Server* srv = (Server*)client->_server;

	srv->increaseThreadCounter();

    // Receive until the peer closes the connection
    do {
		iResult = client->_socket->Receive(recvbuf, DEFAULT_BUFLEN);
        if (iResult == 0) {
			printf("Client_%d: %s\n", id, recvbuf);
			
			Info command_info;
			if (Info::parseXML(recvbuf, command_info) != 0) {
				return 1;
			}

			srv->performCommand(id, &command_info);
		}
		
		//clear the buffer
		for (int i = 0, length = sizeof(recvbuf); i < length; ++i) {
			recvbuf[i] = 0;
		}
    }
	while(iResult == 0);

	Info command_to_server;
	command_to_server.setParam("code", util::itos(Info::CLIENT_CODE_CLOSE));
	srv->performCommand(id, &command_to_server);

	srv->decreaseThreadCounter();
	ExitThread(1);
	return 0;
}

int ClientProxy::stop()
{
	printf("Client_%d is closed.\n", this->getId());
	this->_socket->Close();

	return 0;
}

int ClientProxy::sendToClient(string sendbuf)
{
	if (_socket->Send(sendbuf.c_str()) != 0) {
		return 1;
	}
	return 0;
}

void ClientProxy::setId(int id)
{
	this->_id = id;
}

int ClientProxy::getId()
{
	return this->_id;
}
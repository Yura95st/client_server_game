#pragma once

#include "util.h"
#include "MySocket.h"

class Client
{
public:
	Client(void);
	~Client(void);
	int Start();
	int Stop();
	int Shutdown();
	void printHelp();

	static void printMatrix(string matrix, int matrixSize);

	friend DWORD threadProcedure(Client* client);

private:
	static const char* _ip;
	static const int _port;
	static const int _family;
	MySocket* _socket;	
};

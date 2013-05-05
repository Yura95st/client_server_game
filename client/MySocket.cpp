#include "MySocket.h"


MySocket::MySocket(int family, const char* ip, int port)
	:_socket(0), _family(family), _ip(ip), _port(port)
{
}


MySocket::~MySocket(void)
{
}

int MySocket::Create()
{
	WSADATA wsaData = {0};

	// Initialize Winsock (initiate use of the Winsock DLL by a process)
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// Create a SOCKET for listening for incoming connection requests.
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET) {
		//wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
		return 2;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	service.sin_family = _family;
	service.sin_addr.s_addr = inet_addr(_ip);// or htonl(INADDR_ANY);
	service.sin_port = htons(_port);

	// Connect to server.
	if (connect(_socket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
		//printf("Unable to connect to server: %ld\n", WSAGetLastError());
		this->Close();        
        return 3;
    }

	return 0;
}

int MySocket::Close()
{
    if (closesocket(_socket) == SOCKET_ERROR) {
        //wprintf(L"close failed with error: %d\n", WSAGetLastError());
        return 1;
    }
	return 0;
}

int MySocket::GetLastErrorCode()
{
	return WSAGetLastError();
}

int MySocket::Send(const char* sendbuf)
{
	if (send(_socket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) {
		//wprintf(L"send failed with error: %d\n", WSAGetLastError());
		this->Close();
		return 1;
	}
	return 0;
}

int MySocket::Receive(char* recvbuf, int buf_len)
{
	int iResult = recv(_socket, recvbuf, buf_len, 0);

	if (iResult < 0) {
		//wprintf(L"recv failed with error: %d\n", WSAGetLastError());
		return 2;
	}
	else if (iResult == 0) {
		//wprintf(L"Connection closed\n");
		return 1;
	}
	//wprintf(L"Bytes received: %d\n", iResult);
	return 0;
}


int MySocket::ShutDown(int how)
{
    // shutdown the connection since no more data will be sent
    if (shutdown(_socket, how) == SOCKET_ERROR) {
        //printf("shutdown failed: %d\n", WSAGetLastError());
		this->Close();
        return 1;
    }
	return 0;
}
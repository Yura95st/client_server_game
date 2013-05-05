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

	// Bind the socket.
	if (bind(_socket, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR) {
		//wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
		this->Close();
		return 3;
	}

	return 0;
}

int MySocket::Listen()
{
	if (listen(_socket, 5)  == SOCKET_ERROR) {
		//wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		this->Close();
		return 1;
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

SOCKET MySocket::GetSocket()
{
	return _socket;
}
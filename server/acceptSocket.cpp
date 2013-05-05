#include "acceptSocket.h"


AcceptSocket::AcceptSocket(MySocket* socket)
	:_socket(socket)
{
}


AcceptSocket::~AcceptSocket(void)
{
}


int AcceptSocket::Create()
{
	_accept_socket = accept(_socket->GetSocket(), NULL, NULL);
	if (_accept_socket == INVALID_SOCKET) {
		//wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
		this->Close();
		return 1;
	}
	return 0;
}

int AcceptSocket::Close()
{
    if (closesocket(_accept_socket) == SOCKET_ERROR) {
        //wprintf(L"close failed with error: %d\n", WSAGetLastError());
        return 1;
    }
	return 0;
}

int AcceptSocket::GetLastErrorCode()
{
	int iResult = WSAGetLastError();
	return iResult;
}

int AcceptSocket::Send(const char* sendbuf)
{
	if (send(_accept_socket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) {
		//wprintf(L"send failed with error: %d\n", WSAGetLastError());
		this->Close();
		return 1;
	}
	return 0;
}

int AcceptSocket::Receive(char* recvbuf, int buf_len)
{
	int iResult = recv(_accept_socket, recvbuf, buf_len, 0);

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
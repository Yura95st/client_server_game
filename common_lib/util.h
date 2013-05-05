#pragma once
#ifndef SERVER_MYLIBS_H_

#define SERVER_MYLIBS_H_

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
using namespace std;


class util
{
public:
	util(void);
	~util(void);

	static string itos(int i);
};

#endif


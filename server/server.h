#pragma once
#ifndef SERVER_server_H_

#define SERVER_server_H_

#include "util.h"
#include "clientproxy.h"
#include "game.h"
#include "info.h"
#include <map>
#include <vector>

class Server
{
public:
	Server(void);
	~Server(void);
	int start();
	int stop();
	void performCommand(int sender_id, Info *command_info);
	int getThreadCounter();
	void increaseThreadCounter();
	void decreaseThreadCounter();			

private:
	static const char* _ip;
	static const int _port;
	static const int _family;	
	static int _thread_counter;		
	vector<Game> _games;	
	vector<ClientProxy*> _clients;

	int _nextPlayerId;

	int getNextPlayerId();
	ClientProxy* getClientById(int client_id);
	
	int performStepCommand(int client_id, Info *command_info);
	int performGiveUpCommand(int senderId);
	int performDrawProposeCommand(int senderId);
	int performDrawApproveCommand(int sender_id);
	int performDrawRejectCommand(int sender_id);

	Game* getGameByPlayerId(int player_id);	
	int getOpponetByPlayerId(int player_id);
	void sendResult(int clientId, int code);
	void closeClient(int client_id);
	void closeGame(int client_id);
};

#endif
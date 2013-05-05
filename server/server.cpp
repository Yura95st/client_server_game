#include "server.h"
#include "MySocket.h"
#include "info.h"
#include "game.h"
#include <sstream>
#include <algorithm>
#include "util.h"

using namespace std;

const char* Server::_ip = "127.0.0.1";
//const char* Server::_ip = "192.168.1.2";
const int Server::_port = 27015;
const int Server::_family = AF_INET;
int Server::_thread_counter = 0;

Server::Server(void)
	:_nextPlayerId(0)
{		
}

Server::~Server(void)
{
}

int Server::getNextPlayerId(){	
	return ++_nextPlayerId;
}

int Server::start()
{
	int iResult = 0;
	MySocket* socket = new MySocket(_family, _ip, _port);
	
	if (socket->Create() != 0) {
		return socket->GetLastErrorCode();
	}

	// Listen for incoming connection requests on the created socket.
	if (socket->Listen() != 0) {
		return socket->GetLastErrorCode();
	}	
	
	wprintf(L"Waiting for first client to connect...\n");
	do {		
		// Accept the connection 1.
		AcceptSocket* accept_socket_1 = new AcceptSocket(socket);
		if (accept_socket_1->Create() != 0) {
			return accept_socket_1->GetLastErrorCode();
		}

		int player_id = getNextPlayerId();
		ClientProxy* client_1 = new ClientProxy();
		client_1->setId(player_id );
		client_1->create(accept_socket_1, this);		
		_clients.push_back(client_1);
		
		Game* game = NULL;
		//find game with one player	
		for(int i=0; i < _games.size(); i++) {		
			if(_games[i].getPlayer2() < 0) {
				game = &_games[i];
				break;
			}
		}
				
		if(game == NULL) {
			game = new Game(player_id);
			_games.push_back(*game);
			sendResult(player_id , Info::SERVER_CODE_WAIT_APPONENT);
			wprintf(L"Waiting for second client to connect...\n");
		}
		else {
			//start game
			game->setOpponent(player_id);
		    sendResult(player_id, Info::SERVER_CODE_WAIT_APPONENT_STEP);
			sendResult(game->getPlayer1(), Info::SERVER_CODE_YOUR_STEP);

			wprintf(L"Waiting for other clients to connect...\n");
			game = NULL;
		}			
	}
	while (_thread_counter > 0);

	return 0;
}

void Server::performCommand(int sender_id, Info *command_info)
{	
	string commandCode = command_info->getParam(Info::PARAM_NAME_CODE);

	switch(atoi(commandCode.c_str()))
	{
	case Info::CLIENT_CODE_STEP :
		{
			//STEP	
			performStepCommand(sender_id, command_info);			
			break;
		}

	case Info::CLIENT_CODE_GIVE_UP :
		{
			//GIVE UP
			performGiveUpCommand(sender_id);
			break;
		}

	case Info::CLIENT_CODE_DRAW_PROPOSE :
		{
			//DRAW PROPOSE
			performDrawProposeCommand(sender_id);
			break;
		}

	case Info::CLIENT_CODE_DRAW_APPROVE :
		{
			//DRAW APPROVE
			performDrawApproveCommand(sender_id);
			break;
		}

	case Info::CLIENT_CODE_DRAW_REJECT :
		{
			//DRAW REJECT
			performDrawRejectCommand(sender_id);
			break;
		}

	case Info::CLIENT_CODE_CLOSE :
		{
			//CLOSE THE CLIENT		
			int opponent_id = getOpponetByPlayerId(sender_id);
			if (opponent_id >= 0) {
				sendResult(opponent_id, Info::SERVER_CODE_OPPONENT_DISCONNECTED);
			}
			closeClient(sender_id);
			closeGame(sender_id);
			break;
		}

	default :
		{
			sendResult(sender_id, Info::SERVER_CODE_INVALID_COMMAND);
			break;
		}
	}
}

void Server::closeGame(int client_id)
{
	for(int i=0; i < _games.size(); i++) {		
		if(_games[i].getPlayer1()==client_id ||_games[i].getPlayer2()==client_id) {
			_games.erase(_games.begin()+i);
			break;
		}
	}
}

int Server::performStepCommand(int sender_id, Info *command_info)
{	
	int x = stoi(command_info->getParam(Info::PARAM_NAME_X));
	int y = stoi(command_info->getParam(Info::PARAM_NAME_Y));

	Game *game = getGameByPlayerId(sender_id);

	if(game == NULL) {
		sendResult(sender_id, Info::SERVER_CODE_WAIT_APPONENT);
		return 0;
	}

	Step *step = new Step(x, y, sender_id);

	int opponent_id = getOpponetByPlayerId(sender_id);
	
	if (opponent_id < 0) {
		sendResult(sender_id, Info::SERVER_CODE_WAIT_APPONENT);
		return 0;
	}

	//try step
	try {
		game->setCell(*step);
	}
	catch(Game::Exception e) {	
		switch(e.code) {
		case Game::GAME_STEP_OCCUPIED :
		case Game::GAME_STEP_INVALID :
			sendResult(sender_id, Info::SERVER_CODE_STEP_FAILED);
			break;
		case Game::GAME_STEP_NOT_YOUR_TURN :
			sendResult(sender_id, Info::SERVER_CODE_NOT_YOUR_TURN);
			break;
		}						
		return 0;
	};

	//check game state
	try {
		game->checkGameState();
	}
	catch (Game::Exception e) {
		if (e.code == Game::GAME_STATE_WIN) {
			//is win
			sendResult(sender_id, Info::SERVER_CODE_WIN);
			//send to opponent
			sendResult(opponent_id, Info::SERVER_CODE_LOSS);
		} 
		else if (e.code == Game::GAME_STATE_DRAW) {
			sendResult(sender_id, Info::SERVER_CODE_DRAW);
			sendResult(opponent_id, Info::SERVER_CODE_DRAW);
		}

		closeGame(sender_id);

		//close clients
		closeClient(sender_id);
		closeClient(opponent_id);

		return 0;
	};

	//send result to current player
	sendResult(sender_id, Info::SERVER_CODE_WAIT_APPONENT_STEP);
	//send result to opponent
	sendResult(opponent_id, Info::SERVER_CODE_YOUR_STEP);

	return 1;
}

int Server::performGiveUpCommand(int sender_id)
{
	int opponent_id = getOpponetByPlayerId(sender_id);
	sendResult(sender_id, Info::SERVER_CODE_LOSS);
	
	if (opponent_id >= 0) {
		sendResult(opponent_id, Info::SERVER_CODE_WIN);
	}	

	//close clients
	closeClient(sender_id);
	closeClient(opponent_id);

	closeGame(sender_id);
	return 0;
}

int Server::performDrawProposeCommand(int sender_id)
{
	int opponent_id = getOpponetByPlayerId(sender_id);
	if (opponent_id < 0) {
		return 1;
	}	
	
	Game *game = getGameByPlayerId(sender_id);
	if(game == NULL) {
		return 1;
	}

	game->setdrawRequest(sender_id);
	sendResult(opponent_id, Info::SERVER_CODE_DRAW_PROPOSE_APPEARED);
	return 0;
}

int Server::performDrawApproveCommand(int sender_id)
{
	int opponent_id = getOpponetByPlayerId(sender_id);
	if (opponent_id < 0) {
		return 1;
	}	
	
	Game *game = getGameByPlayerId(sender_id);
	if(game == NULL) {
		return 1;
	}

	if (opponent_id != game->getdrawRequest()) {
		return 1;
	}

	sendResult(sender_id, Info::SERVER_CODE_DRAW);
	sendResult(opponent_id, Info::SERVER_CODE_DRAW);

	closeGame(sender_id);

	//close clients
	closeClient(sender_id);
	closeClient(opponent_id);
	return 0;
}

int Server::performDrawRejectCommand(int sender_id)
{
	int opponent_id = getOpponetByPlayerId(sender_id);
	if (opponent_id < 0) {
		return 1;
	}	
	
	Game *game = getGameByPlayerId(sender_id);
	if(game == NULL) {
		return 1;
	}

	if (opponent_id != game->getdrawRequest()) {
		return 1;
	}		

	game->setdrawRequest(-1);
	sendResult(opponent_id, Info::SERVER_CODE_DRAW_PROPOSE_REJECTED);
	return 0;
}

void Server::closeClient(int clientId) 
{
	ClientProxy* client = getClientById(clientId);

	if (client != NULL) {
		for(int i=0; i < _clients.size(); i++) {
			if(client==_clients[i]) {
				_clients.erase(_clients.begin()+i);
				break;
			}
		}
		client->stop();
	}
}

int Server::getOpponetByPlayerId(int player_id)
{
	Game *game = getGameByPlayerId(player_id);

	if(game == NULL) {
		return -1;
	}

	return (game->getPlayer1() != player_id) ? game->getPlayer1() : game->getPlayer2();
}

ClientProxy* Server::getClientById(int clientId)
{
	ClientProxy *client = NULL;
	for(int i=0; i < _clients.size(); i++) {
		client = _clients[i];
		if(client->getId()==clientId){			
			break;
		}
	}

	return client;
}

void Server::sendResult(int clientId, int code) 
{		
	Info result_info;
	result_info.setParam(Info::PARAM_NAME_CLIENT_ID, util::itos(clientId));
	result_info.setParam(Info::PARAM_NAME_CODE, util::itos(code));	

	if(code != Info::SERVER_CODE_STEP_FAILED && code != Info::SERVER_CODE_WAIT_APPONENT) {
		Game *game = getGameByPlayerId(clientId);
		result_info.setParam(Info::PARAM_NAME_MATRIX, game->getMatrixAsStr());
		
		string matrixSize = util::itos(game->getMatrixSize());
		result_info.setParam(Info::PARAM_NAME_MATRIX_SIZE, matrixSize);
	}

	string str = result_info.toXML();

	ClientProxy* client = getClientById(clientId);
	
	if (client != NULL) {
		client->sendToClient(str);		
	}
}

Game* Server::getGameByPlayerId(int playerId)
{
	Game *game = NULL;
	for(int i=0; i < _games.size(); i++) {		
		if (_games[i].getPlayer1()==playerId || _games[i].getPlayer2()==playerId) {
			game = &_games[i];
			break;
		}
	}
	return game;
}

int Server::getThreadCounter()
{
	return _thread_counter;
}

void Server::increaseThreadCounter()
{
	++_thread_counter;
}

void Server::decreaseThreadCounter()
{
	--_thread_counter;
}
#include "client.h"
#include "MySocket.h"
#include <sstream>
#include <iostream>
#include "info.h"
#include "util.h"

using namespace std;

#define DEFAULT_BUFLEN 512

const char* Client::_ip = "127.0.0.1";
//const char* Client::_ip = "192.168.1.2";
const int Client::_port = 27015;
const int Client::_family = AF_INET;

Client::Client(void)
{
}


Client::~Client(void)
{
}

int Client::Start()
{
	int iResult = 0;
	char recvbuf[DEFAULT_BUFLEN] = "";

	_socket = new MySocket(_family, _ip, _port);
	
	if (_socket->Create() != 0) {
		return _socket->GetLastErrorCode();
	}

	string str;

	DWORD dwThreadId;
	HANDLE thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&threadProcedure, (LPVOID) this, 0, &dwThreadId);
	
	if (thread_handle == NULL) {
		return 1;
	}

	while (cin >> str) {
		//cout << "Type command: \n";
		Info* command_info = new Info();
		if (str == "quit" || str == "q") {
			break;
		}		
		else if (str == "help" || str == "h" || str == "?") {
			printHelp();
			continue;
		}
		else if (str == "giveup") {
			command_info->setParam(Info::PARAM_NAME_CODE, util::itos(Info::CLIENT_CODE_GIVE_UP));
		}
		else if (str == "step" || "s") {
			int x,y;
			//cout << "Enter coordinates\n";
			if (scanf("%d %d", &x, &y) != 2) {				
				//cout << "Input failed.\n";
				continue;
			}
			command_info->setParam(Info::PARAM_NAME_CODE, util::itos(Info::CLIENT_CODE_STEP));
			command_info->setParam(Info::PARAM_NAME_X, util::itos(x));
			command_info->setParam(Info::PARAM_NAME_Y, util::itos(y));
		}
		else if (str == "draw") {
			command_info->setParam(Info::PARAM_NAME_CODE, util::itos(Info::CLIENT_CODE_DRAW_PROPOSE));
		}
		else if (str == "approve") {
			command_info->setParam(Info::PARAM_NAME_CODE, util::itos(Info::CLIENT_CODE_DRAW_APPROVE));
		}
		else if (str == "reject") {
			command_info->setParam(Info::PARAM_NAME_CODE, util::itos(Info::CLIENT_CODE_DRAW_REJECT));
		}

		string str_command = command_info->toXML();

		if (_socket->Send(str_command.c_str()) != 0) {
			return _socket->GetLastErrorCode();
		}
	}

	if (_socket->ShutDown(SD_SEND) != 0) {
		return _socket->GetLastErrorCode();
	}

	TerminateThread(thread_handle, 1);

	// Receive data from server	
	/*if (_socket->Receive(recvbuf, DEFAULT_BUFLEN) != 0) {
		return _socket->GetLastErrorCode();
	}
	printf("%s\n", recvbuf);*/

	if (_socket->Close() != 0) {
		return _socket->GetLastErrorCode();
	}

	return 0;
}

DWORD threadProcedure(Client* client)
{
	int iResult;
    char recvbuf[DEFAULT_BUFLEN] = "";

    // Receive until the peer closes the connection
    do {
		iResult = client->_socket->Receive(recvbuf, DEFAULT_BUFLEN);
        if (iResult == 0) {
			Info receiveInfo;	
			if (Info::parseXML(recvbuf, receiveInfo) != 0) {
				continue;
			}

			string strCode = receiveInfo.getParam(Info::PARAM_NAME_CODE);
			if (strCode == "") {
				continue;
			}
			
			string matrix = receiveInfo.getParam(Info::PARAM_NAME_MATRIX);
			string matrixSizeStr = receiveInfo.getParam(Info::PARAM_NAME_MATRIX_SIZE);
			int matrixSize = atoi(matrixSizeStr.c_str());

			switch(atoi(strCode.c_str())) 
			{
			case Info::SERVER_CODE_YOUR_STEP :	
				{
					Client::printMatrix(matrix, matrixSize);
					printf("It's your step:\n");
					break;
				}

			case Info::SERVER_CODE_STEP_FAILED :
				{
					printf("Input failed. Please try again.\n");
					break;
				}

			case Info::SERVER_CODE_WAIT_APPONENT :
				{
					printf("Please wait another player.\n");
					break;
				}

			case  Info::SERVER_CODE_WAIT_APPONENT_STEP :
				{
					Client::printMatrix(matrix, matrixSize);
					printf("Please wait step of opponent.\n");
					break;
				}

			case Info::SERVER_CODE_LOSS :
				{
					Client::printMatrix(matrix, matrixSize);
					printf("Game is over. You lose.\n");
					break;
				}

			case  Info::SERVER_CODE_WIN :
				{
					Client::printMatrix(matrix, matrixSize);
					printf("Game is over. You win!\n");
					break;
				}

			case Info::SERVER_CODE_DRAW :
				{
					Client::printMatrix(matrix, matrixSize);
					printf("Game is over. Draw.\n");
					break;
				}

			case Info::SERVER_CODE_OPPONENT_DISCONNECTED :
				{
					printf("Your opponent disconnnected.\n");
					break;
				}

			case Info::SERVER_CODE_NOT_YOUR_TURN :
				{
					printf("It is not your turn.\n");
					break;
				}

			case Info::SERVER_CODE_DRAW_PROPOSE_APPEARED :
				{
					printf("Would You like to play in a draw?.\n");
					break;
				}

			case Info::SERVER_CODE_DRAW_PROPOSE_REJECTED :
				{
					printf("Your draw request was rejected.\n");
					break;
				}

			case Info::SERVER_CODE_INVALID_COMMAND :
				{
					printf("Invalid command.\n");
					break;						
				}
			}
			
			//clear the buffer
			for (int i = 0, length = sizeof(recvbuf); i < length; ++i) {
				recvbuf[i] = 0;
			}
		}
    } 
	while(iResult == 0);

	ExitThread(1);
	return 0;
}

void Client::printMatrix(string matrix, int matrixSize) {
	system("cls");
	printf("\n");

	string varExpressionStr = "---";
	for(int k=1; k < matrixSize; ++k) {
		varExpressionStr += "+---";
	}
	varExpressionStr += "+\n";

	for (int i = 0; i < matrix.size(); i++) {
		char c = matrix[i];
		if(c == '1') {
			c = 'X';
		}
		else if (c == '2') {
			c = '0';
		}
		else if (c == '0') {
			c= ' ';
		}

		if(c == ';') {
			printf("\n");
			printf(varExpressionStr.c_str());
		}
		else {
			printf(" %c |", c);
		}
	}
	
	printf("\n");
}

void Client::printHelp()
{
	printf("\n");
	printf("Usage:\n");
	printf("step [X] [Y]        To make the step\n");
	printf("giveup              To give up\n");
	printf("draw                To make a draw request\n");
	printf("approve             To approve the draw request\n");
	printf("reject              To reject the draw request\n");
	printf("quit                To exit\n");
	printf("\n");
}
#ifndef SERVER_COMMAND_H_

#define SERVER_COMMAND_H_
#include <string>
#include <map>
using namespace std;

class Info
{
public:
	Info(void);
	~Info(void);	
	void setParam(string name, string value);
	string toXML();
	static int parseXML(string str, Info &command_info);	
	string getParam(string param_name);

	static const string PARAM_NAME_X;
	static const string PARAM_NAME_Y;	
	static const string PARAM_NAME_CODE;	
	static const string PARAM_NAME_CLIENT_ID;
	static const string PARAM_NAME_MATRIX;
	static const string PARAM_NAME_MATRIX_SIZE;
			
	static enum CLIENT_CODE	{
		CLIENT_CODE_START_GAME,
		CLIENT_CODE_STEP,
		CLIENT_CODE_GIVE_UP,
		CLIENT_CODE_DRAW_PROPOSE,
		CLIENT_CODE_DRAW_APPROVE,
		CLIENT_CODE_DRAW_REJECT,
		CLIENT_CODE_CLOSE
	};

	static enum SERVER_CODE	{
		SERVER_CODE_WAIT_APPONENT,
		SERVER_CODE_WAIT_APPONENT_STEP,
		SERVER_CODE_YOUR_STEP,
		SERVER_CODE_STEP_FAILED,
		SERVER_CODE_NOT_YOUR_TURN,
		SERVER_CODE_WIN,
		SERVER_CODE_LOSS,
		SERVER_CODE_DRAW,
		SERVER_CODE_DRAW_PROPOSE_APPEARED,
		SERVER_CODE_DRAW_PROPOSE_REJECTED,
		SERVER_CODE_OPPONENT_DISCONNECTED,
		SERVER_CODE_INVALID_COMMAND
	};

private:	
	map<string, string> _params;
};

#endif
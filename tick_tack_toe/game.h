#pragma once
#include "step.h"
#include <string>
#include <map>

using namespace std;

class Game
{
public:
	Game(int _player1);
	~Game(void);

	static enum GAME_STATES{
		GAME_STEP_INVALID,
		GAME_STEP_OCCUPIED,
		GAME_STEP_NOT_YOUR_TURN,
		GAME_STATE_WIN,
		GAME_STATE_DRAW
	};

	int setCell(Step &step);
	int checkGameState();
	void print();
	string toXML();
	int getPlayer1();
	int getPlayer2();
	string getMatrixAsStr();
	int getMatrixSize();	
	void setOpponent(int opponent_id);
	void setdrawRequest(int sender_id);
	int getdrawRequest();
	void cleanDrawRequest();
	
	struct Exception
	{
		string text;
		int code;
		Exception(string _text, int _code)
		{
			text = _text;
			code = _code;
		}
	};

private:
	static const int FIELD_SIZE = 10;
	static const int WINNER_CELLS_VALUE = 5;
	int playing_field[FIELD_SIZE][FIELD_SIZE];
	int _player1;
	int _player2;
	Step* _lastStep;	
	map<int, int> _playerToType;
	int _drawRequest;

	bool isCellOccupied(int x, int y);	
	bool isFieldFull();
	bool isWinnerAppeared();

	bool checkHorizontal();
	bool checkVertical();
	bool checkMainDiagonal();
	bool checkIncidentalDiagonal();
};



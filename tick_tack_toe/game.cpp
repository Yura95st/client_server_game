#include "game.h"
#include <string>
#include <sstream>

using namespace std;

Game::Game(int player1)
	: _player1(player1), _player2(-1), _lastStep(NULL), _drawRequest(-1)
{
	memset(playing_field, 0, sizeof(playing_field));
	_playerToType.insert(pair<int, int>(player1, 1));
}

Game::~Game(void)
{
}

int Game::getPlayer1(){
	return _player1;
}

int Game::getPlayer2(){
	return _player2;
}

void Game::setOpponent(int opponent_id)
{
	_player2 = opponent_id;
	_playerToType.insert(pair<int, int>(_player2, 2));
	//Set default last step before the first one
	_lastStep = new Step(-1, -1, opponent_id);
}

int Game::setCell(Step &step)
{
	int x = step.getX();
	int y = step.getY();
	int type = _playerToType[step.getPlayerId()];

	this->cleanDrawRequest();

	if (x < 0 || x >= FIELD_SIZE || y < 0 || y >= FIELD_SIZE) {
		throw (Exception("Step is invalid.\n", Game::GAME_STEP_INVALID));
		//return 1;
	}

	//check turn of step
	if(_lastStep != NULL && _lastStep->getPlayerId() == step.getPlayerId()){
		throw (Exception("It is not your turn.\n", Game::GAME_STEP_NOT_YOUR_TURN));
	}


	if (isCellOccupied(x, y)) {
		throw (Exception("Cell is occupied.\n", Game::GAME_STEP_OCCUPIED));
		//return 2;
	}

	playing_field[x][y] = type;
	
	this->_lastStep = &step;

	return 0;
}

bool Game::isCellOccupied(int x, int y)
{
	return (playing_field[x][y] != 0);
}

int Game::checkGameState()
{
	if (isWinnerAppeared()) {
		//Convert int to string
		stringstream ss;
		ss << _lastStep->getPlayerId();

		string str = "Winner is player_" + ss.str();
		str += "\n";
		throw (Exception(str, Game::GAME_STATE_WIN));
		//return 2;
	}

	if (isFieldFull()) {
		throw (Exception("Game is over in a draw.\n", Game::GAME_STATE_DRAW));
		//return 1;
	}

	return 0;
}

bool Game::isFieldFull()
{
	for (int i=0; i<FIELD_SIZE; ++i) {
		for (int j=0; j<FIELD_SIZE; ++j) {
			if (playing_field[i][j] == 0) {
				return false;
			}
		}
	}
	return true;
}

bool Game::isWinnerAppeared()
{
	return (checkVertical() || checkHorizontal() || checkMainDiagonal() || checkIncidentalDiagonal());
}

bool Game::checkHorizontal()
{
	int counter=0;
	int x = _lastStep->getX();
	int y = _lastStep->getY();
	int type = _playerToType[_lastStep->getPlayerId()];

	while(y >= 0 && playing_field[x][y] == type) {
		++counter;
		--y;
	}

	if (counter < WINNER_CELLS_VALUE) {
		y += counter + 1;
		while(y < FIELD_SIZE && playing_field[x][y] == type) {
			++counter;
			++y;
		}		
	}

	return (counter == WINNER_CELLS_VALUE);
}

bool Game::checkVertical()
{
	int counter=0;
	int x = _lastStep->getX();
	int y = _lastStep->getY();
	int type = _playerToType[_lastStep->getPlayerId()];

	while(x >= 0 && playing_field[x][y] == type) {
		++counter;
		--x;
	}

	if (counter < WINNER_CELLS_VALUE) {
		x += counter + 1;
		while(x < FIELD_SIZE && playing_field[x][y] == type) {
			++counter;
			++x;
		}		
	}

	return (counter == WINNER_CELLS_VALUE);
}

bool Game::checkMainDiagonal()
{
	int counter=0;
	int x = _lastStep->getX();
	int y = _lastStep->getY();
	int type = _playerToType[_lastStep->getPlayerId()];

	while(x >= 0 && y >= 0 && playing_field[x][y] == type) {
		++counter;
		--x;
		--y;
	}

	if (counter < WINNER_CELLS_VALUE) {
		x += counter + 1;
		y += counter + 1;
		while(x < FIELD_SIZE && y < FIELD_SIZE && playing_field[x][y] == type) {
			++counter;
			++x;
			++y;
		}
	}

	return (counter == WINNER_CELLS_VALUE);
}

bool Game::checkIncidentalDiagonal()
{
	int counter=0;
	int x = _lastStep->getX();
	int y = _lastStep->getY();
	int type = _playerToType[_lastStep->getPlayerId()];

	while(x >= 0 && y < FIELD_SIZE && playing_field[x][y] == type) {
		++counter;
		--x;
		++y;
	}

	if (counter < WINNER_CELLS_VALUE) {
		x += counter + 1;
		y -= counter + 1;
		while(x < FIELD_SIZE && y >= 0 && playing_field[x][y] == type) {
			++counter;
			++x;
			--y;
		}
	}

	return (counter == WINNER_CELLS_VALUE);
}

void Game::print()
{
	system("cls");
	printf("\n");

	string varExpressionStr = "---";
	for(int k=1; k < FIELD_SIZE; ++k) {
		varExpressionStr += "+---";
	}
	varExpressionStr += "+\n";
	
	char c;

	for(int i=0; i<FIELD_SIZE; ++i) {
		for(int j=0; j<FIELD_SIZE; ++j) {
			if (playing_field[i][j] == 1) {
				c = 'X';
			}
			else if (playing_field[i][j] == 2) { 
				c = '0';
			}
			else {
				c = ' ';
			}
			printf(" %c |", c);
		}
		printf("\n");
		printf(varExpressionStr.c_str());		
	}
	printf("\n");
}

string Game::toXML(){

	stringstream ss;
	ss << FIELD_SIZE;
	
	string str = "<matrix><size>" + ss.str() + "</size>" + "<value>";
	
	for(int i=0; i<FIELD_SIZE; ++i) {
		for(int j=0; j<FIELD_SIZE; ++j) {
			ss.clear();
			ss << playing_field[i][j];
			str += ss.str();			
		}
		str +=';';		
	}
	str += "</value></matrix>";	
	return str;
}

int Game::getMatrixSize(){
	return FIELD_SIZE;
}

string Game::getMatrixAsStr(){
			
	string str;
	
	for(int i=0; i<FIELD_SIZE; ++i) {
		for(int j=0; j<FIELD_SIZE; ++j) {
			stringstream ss;
			ss << playing_field[i][j];
			str += ss.str();			
		}
		str +=';';		
	}
	
	return str;
}

void Game::setdrawRequest(int sender_id)
{
	if (sender_id >= 0) {
		this->_drawRequest = sender_id;
	}
}

void Game::cleanDrawRequest()
{
	this->_drawRequest = -1;
}

int Game::getdrawRequest()
{
	return this->_drawRequest;
}
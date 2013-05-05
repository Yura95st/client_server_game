#pragma once
class Step
{
public:
	Step(void);
	Step(int x, int y, int playerId);
	~Step(void);
	int getX();
	int getY();
	int getPlayerId();

private:
	int _x;
	int _y;
	int _playerId;
};


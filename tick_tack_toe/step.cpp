#include "step.h"

Step::Step(void)
{
}

Step::Step(int x, int y, int playerId)
	:_x(x), _y(y), _playerId(playerId)
{
}

Step::~Step(void)
{
}

int Step::getX()
{
	return _x;
}

int Step::getY()
{
	return _y;
}

int Step::getPlayerId()
{
	return _playerId;
}
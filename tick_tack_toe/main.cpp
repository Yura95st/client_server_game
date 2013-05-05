#include <iostream>
#include "game.h"
#include "step.h"
using namespace std;

int main(int argc, char *argv[])
{
	Game game(1);
	game.setOpponent(2);
	int x, y, type=1;

	while (true) {
		cout << "#Step of player_" << type << "\n";

		if (scanf("%d %d", &x, &y) != 2) {
			cout << "Input failed.\n";
			continue;
		}

		Step *step = new Step(x, y, type);
		
		try {
			game.setCell(*step);
		}
		catch(Game::Exception e)
		{
			cout << e.text;
			continue;
		};

		game.print();

		try {
			game.checkGameState();
		}
		catch(Game::Exception e)
		{
			cout << e.text;
			break;
		};

		//Change the player
		type = (type == 1)? 2 : 1;
	}

	system("pause");
	return 0;
}
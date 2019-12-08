#include <cstdlib>

#include "game.h"
#include "util.h"

using namespace std;

int main()
{
	Game *maze = Game::Instance();
	Util *util = Util::Instance();

	while (true) {
		maze->DrawTitle();
		switch (maze->DrawMenu()) {
			case 0:
				maze->CreateMap();
				maze->DrawMap();
				maze->Start();
				break;
			case 1:
				maze->DrawScore();
				break;
			case 2: return 0;
			default: return 1;
		}

		util->TextColor(Color::kWhite, Color::kBlack);
		system("CLS");
	}
}
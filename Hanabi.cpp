// Hanabi.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "Player.h"
#include "Game.h"

using namespace std;


int main()
{
	int total = 0;
	Game g(true);
	for (int i = 0; i < 1000; i++)
	{
		Player p1;
		Player p2;
		g.setup(p1, p2);
		total += g.gameLoop();
	}
	cout << "Average score " << total/1000.0 << "." << endl;
	return 0;
}


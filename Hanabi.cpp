// Hanabi.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "Player.h"
#include "Game.h"

using namespace std;


int main()
{
	int total = 0;
	int max = 0;
	int temp = 0;
	Game g(false);
	for (int i = 0; i < 1000; i++)
	{
		Player p1;
		Player p2;
		g.setup(p1, p2);
		temp = g.gameLoop();
		if (temp > max)
			max = temp;
		total += temp;
	}
	cout << "Average score " << total/1000.0 << "." << endl;
	cout << "Max score " << max << "." << endl;
	return 0;
}


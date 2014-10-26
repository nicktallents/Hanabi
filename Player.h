#include "GameConstants.h"
#include "Card.h"
#include "Events.h"
#include <vector>
#include <iostream>
using namespace std;
using std::vector;


#ifndef PLAYER_H
#define PLAYER_H

struct CardInfo
{
	CardInfo() : perceivedNum(-1), perceivedColor(-1), usable(false), discardable(true) {}
	int perceivedNum;
	int perceivedColor;
	bool usable;
	bool discardable;
};

class Player
{
public:
	Player();
	Player(const Player& p);
	void tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize);
	Event* ask();
private:
	vector<CardInfo> KB;		// my knowledge base
	vector<CardInfo> oKB;		// partner's knowledge base
	vector<Card> oHand;			// partner's known hand for purpose of knowing swap movements
	vector<Card> discardPile;	// game's discard pile
	int nHints;					// number of hints
	int nFuses;					// number of fuses
	vector<int> board;			// board
};

#endif

#include "Player.h"

Player::Player() {
	for(int i=0;i<HAND_SIZE;i++) {
		CardInfo temp();
		KB.push_back(temp);
		oKB.push_back(temp);

		Card temp2();
		oHand.push_back(temp2);
	}
}
Player::Player(const Player& p) {
	KB = p.KB;
	oKB = p.oKB;
	oHand = p.oHand;
	discardPile = p.discardPile;
}

Event* Player::ask()
{
	if(oHand[HAND_SIZE-1].value == 5 && oKB[HAND_SIZE].perceivedNum != 5) {
		//If hints greater than zero
		//Tell other player of 5 in discard slot
	}

	if(KB[HAND_SIZE-1].perceivedNum == 5) {
		//Swap my 5 in discard slot with another card
	}


	Event *e = new Event();
	return e;
}

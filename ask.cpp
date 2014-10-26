
#include "Player.h"

Player::Player() {
	for(int i=0;i<HAND_SIZE;i++) {
		CardInfo temp;
		KB.push_back(temp);
		oKB.push_back(temp);

		Card temp2;
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

	/*Ask order:
	Check discard slots to see if valuable cards are in danger


	if you know you have a valid play, and your opponent knows he has a valid play, play a card
	if you know you have a valid play, and your opponent has one but doesn't know, use a guess
	if you know you have a valid play, and your opponent does not have a valid play, play a card
	if you don't know you have a valid play, and your opponent has one but doesn't know, use a guess
	if you don't know you have a valid play and your oppoent knows he has a valid play, discard
	if you don't know you have a valid play and your oppoent does not have a valid play, discard. 
	If, on your turn, your opponent does not know of a valid play that he possesses, inform him.
	Otherwise, play your legal move, or discard a card. 


	*/
	if(checkDiscardSlot(oHand[HAND_SIZE-1])) {
		//If hints greater than zero
		//Tell other player of valuable card in discard slot
	}
	if(checkDiscardSlot(KB[HAND_SIZE-1])) {
		//Swap valuable card from discard slot
	}

	//If both players know of valid moves
	if(knowsValidPlay(KB) && knowsValidPlay(oKB)) {
		//Play a valid card
	}
	//If I know of valid move, other player doesn't but has one
	else if(knowsValidPlay(KB) && !(knowsValidPlay(oKB)) && hasValidPlay(oHand)) {
		//Use guess to tell other player of valid move
	}
	else if(knowsValidPlay(KB) && !(knowsValidPlay(oKB)) && !(hasValidPlay(oHand)) {
		//Play a valid card
	}

	Event *e = new Event();
	return e;
}

bool Player::checkDiscardSlot(CardInfo k) {
	if(remainingCount(k.perceivedNum, k.perceivedColor) == 1) {
		return true;
	}
	return false;
}

bool Player::checkDiscardSlot(Card k) {
	if(remainingCount(k.number, k.color) == 1) {
		return true;
	}
	return false;
}


//Expects positive val/color
//Gets the remaining count of a card by value/color
int Player::remainingCount(int val, int color) {
	int count = 0;
	for(int i = 0; i < HAND_SIZE; i++) {
		if(oHand[i].number == val && oHand[i].color == color) {
			count++;
		}
	}
	for(int i = 0; i < discardPile.size(); i++) {
		if(discardPile[i].number == val && discardPile[i].color == color) {
			count++;
		}
	}
	if(val == 1) {
		return 3 - count;
	}
	if(val == 2 || val == 3 || val == 4) {
		return 2 - count;
	}
	if(val == 5) {
		return 1  - count;
	}
	return -1;
}

bool Player::knowsValidPlay(vector<CardInfo> kb) {
	for(int i = 0; i < kb.size(); i++) {
		if(kb[i].usable) {
			return true;
		}
	}
	return false;
}

bool Player::hasValidPlay(vector<Card> hand) {
	for(int i = 0; i < hand.size(); i++) {
		int val = hand[i].number;
		int col = hand[i].color;

		//Maintain list of needed next cards for easier checks?

		//If card in hand is legal
		//return true;

	}
	return false;
}
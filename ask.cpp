
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
	if(checkDiscardSlot(oHand[0])) {
		if(nHints > 0) {
			if(oKB[0].perceivedNum == -1) {
				vector<int> numIndices;
				for(int i = 0; i< HAND_SIZE; i++) {
					if(oHand[i].number == oHand[0].number) {
						numIndices.push_back(i);
					}
				}

				NumberHintEvent* hint = new NumberHintEvent(numIndices, oHand[0].number);
				return hint;

			}
			else if(oKB[0].perceivedColor == -1) {
				vector<int> colorIndices;
				for(int i = 0; i < HAND_SIZE; i++) {
					if(oHand[i].color == oHand[0].color) {
						colorIndices.push_back(i);
					}
				}
				ColorHintEvent* hint = new ColorHintEvent(colorIndices, oHand[0].color);
				return hint;
			}
				
				//Tell other player of valuable card
		}
	}
	if(checkDiscardSlot(KB[0])) {
		for(int i = 1; i < HAND_SIZE; i++) {
			if(KB[i].discardable) {
				SwapEvent* swap = new SwapEvent(0,i);
				return swap;
			}
		}
		//Swap valuable card from discard slot
	}

	//If both players know of valid moves
	if(knowsValidPlay(KB) && knowsValidPlay(oKB)) {
		for(int i = 0; i < HAND_SIZE; i++) {
			if(KB[i].usable) {
				PlayEvent* play = new PlayEvent(i);
				return play;
			}
		}
		//Play a valid card
	}
	//If I know of valid move, other player doesn't but has one
	else if(knowsValidPlay(KB) && !(knowsValidPlay(oKB)) && hasValidPlay(oHand)) {
		//Use guess to tell other player of valid move
	}
	else if(knowsValidPlay(KB) && !(knowsValidPlay(oKB)) && !(hasValidPlay(oHand))) {
		for(int i = 0; i < HAND_SIZE; i++) {
			if(KB[i].usable) {
				PlayEvent* play = new PlayEvent(i);
				return play;
			}
		}
		//Play a valid card
	}
	else if(!(knowsValidPlay(KB)) && knowsValidPlay(oKB)) {
		DiscardEvent* de = new DiscardEvent(0);
		return de;
		//Discard from your discard slot
	}
	else if(!(knowsValidPlay(KB)) && !(knowsValidPlay(oKB)) && hasValidPlay(oHand)) {
		//use guses to tell other player of valid move
	}
	else if(!(knowsValidPlay(KB)) && !(knowsValidPlay(oKB)) && !(hasValidPlay(oHand))) {
		DiscardEvent* de = new DiscardEvent(0);
		return de;
		//Discard from discard slot
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

	//Check other player hand
	for(int i = 0; i < HAND_SIZE; i++) {
		if(oHand[i].number == val && oHand[i].color == color) {
			count++;
		}
	}

	//Check discard
	for(int i = 0; i < discardPile.size(); i++) {
		if(discardPile[i].number == val && discardPile[i].color == color) {
			count++;
		}
	}

	//Check board
	for(int i = 0; i < board.size(); i++) {
		if(color == i && val <= board[i]) {
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
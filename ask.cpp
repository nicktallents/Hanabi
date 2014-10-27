
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
			if(oKB[0].discardable) {
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
		int type = 0;
		Event* guess = validGuess(type);
		if(type == 0) {
			NumberHintEvent* hint = static_cast<NumberHintEvent*>(guess);
			return hint;
		}
		else if(type == 1) {
			ColorHintEvent* hint = static_cast<ColorHintEvent*>(guess);
			return hint;
		}
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
		//use guess to tell other player of valid move
		int type = 0;
		Event* guess = validGuess(type);
		if(type == 0) {
			NumberHintEvent* hint = static_cast<NumberHintEvent*>(guess);
			return hint;
		}
		else if(type == 1) {
			ColorHintEvent* hint = static_cast<ColorHintEvent*>(guess);
			return hint;
		}
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

Event* Player::validGuess(int& type) {
	vector<Card> neededCards;
	for(int i = 0; i < board.size(); i++) {
		if(board[i] < 5) {
			Card temp(i,board[i]+1);
			neededCards.push_back(temp);
		}
	}
	//Make list of needed cards from board

	vector<Card> validCards;
	vector<int> validCardsLoc;
	for(int i = 0; i < oHand.size(); i++) {
		for(int j = 0; j < neededCards.size(); j++) {
			if(oHand[i] == neededCards[j]) {
				validCards.push_back(oHand[i]);
				validCardsLoc.push_back(i);
			}
		}
	}
	//Look through hand and find valid played cards
	int bestCard = 0;
	vector<CardCommon> commonCards;
	vector<int> commonNumbers;
	vector<int> commonColors;
	vector<int> bestCommonNumbers;
	vector<int> bestCommonColors;
	for(int i = 0; i < validCards.size(); i++) {
		CardCommon c;
		commonCards.push_back(c);
		commonCards[i].cardIndex = validCardsLoc[i];
		for(int j = 0; j < oHand.size(); i++) {
			if(validCards[i].color == oHand[j].color) {
				commonCards[i].commonColors.push_back(j);
			}
			if(validCards[i].number == oHand[j].number) {
				commonCards[i].commonNumbers.push_back(j);
			}
		}
	}
	int smallest = 6;
	for(int i = 0; i< commonCards.size(); i++) {
		if(commonCards[i].commonColors.size() < smallest) {
			smallest = commonCards[i].commonColors.size();
			bestCard = i;
		}
		if(commonCards[i].commonNumbers.size() < smallest) {
			smallest = commonCards[i].commonNumbers.size();
			bestCard = i;
		}
	}
	if(commonCards[bestCard].commonColors.size() < commonCards[bestCard].commonNumbers.size()) {
		if(oKB[commonCards[bestCard].cardIndex].perceivedColor == -1) {
			ColorHintEvent* hint = new ColorHintEvent(commonCards[bestCard].commonColors,oHand[commonCards[bestCard].cardIndex].color);
			type = 1;
			return hint;
		}
		else {
			NumberHintEvent* hint = new NumberHintEvent(commonCards[bestCard].commonNumbers, oHand[commonCards[bestCard].cardIndex].number);
			type = 0;
			return hint;
		}
	}
	if(commonCards[bestCard].commonNumbers.size() <= commonCards[bestCard].commonNumbers.size()) {
		if(oKB[commonCards[bestCard].cardIndex].perceivedNum == -1) {
			NumberHintEvent* hint = new NumberHintEvent(commonCards[bestCard].commonNumbers, oHand[commonCards[bestCard].cardIndex].number);
			type = 0;
			return hint;
		}
		else {
			ColorHintEvent* hint = new ColorHintEvent(commonCards[bestCard].commonColors, oHand[commonCards[bestCard].cardIndex].color);
			type = 1;
			return hint;
		}
	}
	//For each card
		//Get the count of cards that share color
		//Get the count of cards that share value
		//Smallest set is the card we use

	//If smallest is a color and oKB knows about it
		//Tell them about the value
		//or vice versa

}
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

struct CardCommon
{
	//CardCommon();
	int cardIndex;
	vector<int> commonNumbers;
	vector<int> commonColors;

};

class Player
{
public:
	Player();
	Player(const Player& p);
	void tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize);
	Event* ask();

	bool checkDiscardSlot(CardInfo k);
	bool checkDiscardSlot(Card k);

	int remainingCount(int val, int color);

	bool knowsValidPlay(vector<CardInfo> kb);
	bool hasValidPlay(vector<Card> hand);
	
	Event* validGuess(int& type);

	void updateKB(vector<CardInfo> &kb, Card c);	// only for after playing / discarding. Not for hints.
private:
	vector<CardInfo> KB;		// my knowledge base
	vector<CardInfo> oKB;		// partner's knowledge base
	vector<Card> oHand;			// partner's known hand for purpose of knowing swap movements
	vector<Card> discardPile;	// game's discard pile
	int nHints;					// number of hints
	int nFuses;					// number of fuses
	vector<int> board;			// board
};

// Nick's Code
void Player::tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize)
{
	if (e->getAction() == SWAP) {
		SwapEvent* se = static_cast<SwapEvent*>(e);
		swap(oKB[se->firstPosition], oKB[se->secondPosition]);
		this->oHand = oHand;
		delete se;
	}

	// this only applies to partner's hand
	else if (e->getAction() == DRAW) {
		this->oHand = oHand;
	}

	else if (e->getAction() == PLAY) {
		PlayEvent* pe = static_cast<PlayEvent*>(e);
		// check if the play was legal and update proper vars
		if (pe->legal) {
			this->board = board;
		}
		else {
			this->discardPile.push_back(pe->c);
			this->nFuses = fuses;
			this->nHints = hints;
		}
		// remove the card from either the player's KB or partner's KB and hand and update KB / oKB
		if (pe->wasItThisPlayer) {
			updateKB(oKB, pe->c);
			KB.erase(KB.begin()+pe->position);
			if (deckSize > 0) {
				KB.push_back(CardInfo());
			}
		}
		else {
			updateKB(KB, pe->c);
			oKB.erase(oKB.begin()+pe->position);
			this->oHand = oHand;
			if (deckSize > 0) {
				oKB.push_back(CardInfo());
			}
		}
		delete pe;
	}

	else if (e->getAction() == DISCARD) {
		DiscardEvent *de = static_cast<DiscardEvent*>(e);
		if (de->wasItThisPlayer) {
			updateKB(oKB, de->c);
			KB.erase(KB.begin()+de->position);
			if (deckSize > 0) {
				KB.push_back(CardInfo());
			}
		}
		else {
			updateKB(KB, de->c);
			oKB.erase(oKB.begin()+de->position);
			this->oHand = oHand;
			if (deckSize > 0) {
				oKB.push_back(CardInfo());
			}
		}
		this->nHints = hints;
		delete de;
	}

	else if (e->getAction() == COLOR_HINT) {
		ColorHintEvent* ce = static_cast<ColorHintEvent*>(e);

		// update KB colors
		for (int i=0; i<ce->indices.size(); i++) {
			KB[ ce->indices[i] ].perceivedColor = ce->color; 

			// if know full card info, update information
			if (KB[ ce->indices[i] ].perceivedNum != -1) {
				// if the number is less than the top card number then it is discardable
				if (KB[ ce->indices[i] ].perceivedNum < board[ KB[ ce->indices[i] ].perceivedColor ]) {
					KB[ ce->indices[i] ].discardable = true;
					KB[ ce->indices[i] ].usable = false;
				}
				// if the number is one greater than a playable card then it is usable
				if (KB[ ce->indices[i] ].perceivedNum == board[ KB[ ce->indices[i] ].perceivedColor ] + 1) {
					KB[ ce->indices[i] ].usable = true;
					// check to see if it's the only card of its kind left and if it is then it's not discardable
					if (remainingCount(KB[ ce->indices[i] ].perceivedNum, KB[ ce->indices[i] ].perceivedColor) == 1) {
						KB[ ce->indices[i] ].discardable = false;
					}
					else {
						KB[ ce->indices[i] ].discardable = true;
					}
				}
			}
		}

		// if one of the indices is in the discard slot then it is not usable without full information, 
		// and not discardable in any situation
		if (ce->indices[0] == 0) {
			if (KB[0].perceivedNum == -1) {
				KB[0].usable = false;
			}
			KB[0].discardable == false;
		}

		delete ce;
	}

	else if (e->getAction() == NUMBER_HINT) {
		NumberHintEvent* ne = static_cast<NumberHintEvent*>(e);

		// update KB numbers
		for (int i=0; i<ne->indices.size(); i++) {
			KB[ ne->indices[i] ].perceivedNum = ne->number;

			// if know full card info, update information
			if (KB[ ne->indices[i] ].perceivedColor != -1) {
				// if the number is less than the top card number then it is discardable
				if (KB[ ne->indices[i] ].perceivedNum < board[ KB[ ne->indices[i] ].perceivedColor ]) {
					KB[ ne->indices[i] ].discardable = true;
					KB[ ne->indices[i] ].usable = false;
				}
				// if the number is one greater than a playable card then it is usable
				if (KB[ ne->indices[i] ].perceivedNum == board[ KB[ ne->indices[i] ].perceivedColor ] + 1) {
					KB[ ne->indices[i] ].usable = true;
					// check to see if it's the only card of its kind left and if it is then it's not discardable
					if (remainingCount(KB[ ne->indices[i] ].perceivedNum, KB[ ne->indices[i] ].perceivedColor) == 1) {
						KB[ ne->indices[i] ].discardable = false;
					}
					else {
						KB[ ne->indices[i] ].discardable = true;
					}
				}
			}
		}

		// if only one card hint and don't know the color and not in the discard slot
		if (ne->indices.size() == 1 && KB[ ne->indices[0] ].perceivedColor == -1 && ne->indices[0] != 0) {
			// check if there is a playable spot on the board. If there is, then it's playable
			int count = 0;
			for (int i=0; i<board.size(); i++) {
				if (KB[ ne->indices[0] ].perceivedNum == board[i] + 1) {
					KB[ ne->indices[0] ].usable = true;
					break;
				}
				else {
					count++;
				}
			}
			// number value isn't playable anywhere. Then it is discardable
			if (count == board.size()) {
				KB[ ne->indices[0] ].usable = false;
				KB[ ne->indices[0] ].discardable = true;
			}
		}

		// if more than one hint and don't know the color and not in the discard slot then it is not discardable
		else if (ne->indices.size() > 1 && ne->indices[0] != 0) {
			for (int i=0; i<ne->indices.size(); i++) {
				if (KB[ ne->indices[i] ].perceivedColor == -1) {
					KB[ ne->indices[i] ].discardable = false;
				}
			}
		}

		// if one of the indices is in the discard slot then it is not usable without full information, 
		// and not discardable in any situation
		if (ne->indices[0] == 0) {
			KB[0].discardable = false;
			if (KB[0].perceivedColor == -1) {
				KB[0].usable = false;
			}

			// if the number is less than the top card number then it is discardable
			else if (KB[0].perceivedNum < board[ KB[0].perceivedColor ]) {
				KB[0].discardable = true;
				KB[0].usable = false;
			}
		}

		delete ne;
	}
}

void Player::updateKB(vector<CardInfo> &kb, Card c)
{
	// update kb
	for (int i=0; i<kb.size(); i++) {
		// if there's only one if the card left, then it's not discardable
		if (remainingCount(kb[i].perceivedNum, kb[i].perceivedColor) == 1) {
			kb[i].discardable = false;
		}
		// if it's the same color with same or lesser value
		if (kb[i].perceivedColor == c.color && kb[i].perceivedNum == c.number) {
			kb[i].usable = false;
			kb[i].discardable = true;
		}
		// if it's usable, see if it's not usable / discardable
		if (kb[i].usable) {
			// if it's the same color, but we don't know the number...
			if (kb[i].perceivedColor == c.color && kb[i].perceivedNum == -1) {
				kb[i].usable = false;
			}
			// if it's the same number, but we don't know the color...
			else if (kb[i].perceivedColor == -1 && kb[i].perceivedNum == c.number) {
				kb[i].usable = false;
			}
		}
		// if it's not usable, see if it's usable now
		else {
			if (kb[i].perceivedColor == c.color && kb[i].perceivedNum - 1 == c.number) {
				kb[i].usable == true;
			}
		}
	}
}

// David's Code
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

#endif
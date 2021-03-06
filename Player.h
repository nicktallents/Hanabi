/********************
Group: David Goehring, Nick Tallents
Project: Homework 4 Hanabi Code
Date: 10/29/14

Our implementation is shown to score on average 15-16 points. 
********************/
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

	Event* crucialPlay(vector<Card> oHand);
	Event* myCrucialPlay();
	Event* discardCard();

	void updateKB(vector<CardInfo> &kb, Card c, bool discarded);	// only for after playing / discarding. Not for hints.
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
	this->nHints = hints;
	this->nFuses = fuses;
	this->board = board;
	if (e->getAction() == SWAP) {
		SwapEvent* se = static_cast<SwapEvent*>(e);
		swap(oKB[se->firstPosition], oKB[se->secondPosition]);
		this->oHand = oHand;
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
			if (pe->legal) {
				updateKB(oKB, pe->c, false);
			}
			else {
				updateKB(oKB, pe->c, true);
			}
			KB.erase(KB.begin()+pe->position);
			if (deckSize > 0) {
				KB.push_back(CardInfo());
			}
		}
		else {
			if (pe->legal) {
				updateKB(KB, pe->c, false);
			}
			else {
				updateKB(KB, pe->c, true);
			}
			oKB.erase(oKB.begin()+pe->position);
			this->oHand = oHand;
			if (deckSize > 0) {
				oKB.push_back(CardInfo());
			}
		}
		//delete pe;
	}

	else if (e->getAction() == DISCARD) {
		DiscardEvent *de = static_cast<DiscardEvent*>(e);
		if (de->wasItThisPlayer) {
			updateKB(oKB, de->c, true);
			KB.erase(KB.begin()+de->position);
			if (deckSize > 0) {
				KB.push_back(CardInfo());
			}
		}
		else {
			updateKB(KB, de->c, true);
			oKB.erase(oKB.begin()+de->position);
			this->oHand = oHand;
			if (deckSize > 0) {
				oKB.push_back(CardInfo());
			}
		}
		this->nHints = hints;
		//delete de;
	}

	else if (e->getAction() == COLOR_HINT) {
		ColorHintEvent* ce = static_cast<ColorHintEvent*>(e);

		// update KB colors
		if(ce->indices.size() == 1 && ce->indices[0] != 0) {
			KB[ce->indices[0]].usable = true;
		}
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
				}
				// check to see if it's the only card of its kind left and if it is then it's not discardable
				if (remainingCount(KB[ ce->indices[i] ].perceivedNum, KB[ ce->indices[i] ].perceivedColor) == 1) {
					KB[ ce->indices[i] ].discardable = false;
				}

			}
		}

		// if it's a one card hint and not in the discard slot, then it's usable
		//if (ce->indices.size() == 1) {
		//	KB[ ce->indices[0] ].usable = true;
		//}

		// if one of the indices is in the discard slot then it is not usable without full information, 
		// and not discardable in any situation
		if (ce->indices[0] == 0) {
			if (KB[0].perceivedNum == -1) {
				KB[0].discardable == false; // added-----------------------------------------------------
			}
		}

		//delete ce;
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
				}
				// check to see if it's the only card of its kind left and if it is then it's not discardable
				if (remainingCount(KB[ ne->indices[i] ].perceivedNum, KB[ ne->indices[i] ].perceivedColor) == 1) {
					KB[ ne->indices[i] ].discardable = false;
				}
			}
		}
		// added
		if (ne->indices.size() == 1 && ne->indices[0] != 0 && KB[ ne->indices[0] ].perceivedColor == -1) {
			KB[ ne->indices[0] ].usable = true;
		}
		else if (ne->indices[0] == 0 && KB[0].perceivedColor == -1) {
			KB[0].discardable = false;
		}
		//

		//// if only one card hint and don't know the color and not in the discard slot
		//if (ne->indices.size() == 1 && KB[ ne->indices[0] ].perceivedColor == -1 && ne->indices[0] != 0) {
		//	// check if there is a playable spot on the board. If there is, then it's playable
		//	int count = 0;
		//	for (int i=0; i<board.size(); i++) {
		//		if (KB[ ne->indices[0] ].perceivedNum == board[i] + 1) {
		//			KB[ ne->indices[0] ].usable = true;
		//			break;
		//		}
		//		else {
		//			count++;
		//		}
		//	}
		//	// number value isn't playable anywhere. Then it is discardable
		//	if (count == board.size()) {
		//		KB[ ne->indices[0] ].usable = false;
		//		KB[ ne->indices[0] ].discardable = true;
		//	}
		//}

		//// if more than one hint and don't know the color and not in the discard slot then it is not discardable
		//else if (ne->indices.size() > 1 && ne->indices[0] != 0) {
		//	for (int i=0; i<ne->indices.size(); i++) {
		//		if (KB[ ne->indices[i] ].perceivedColor == -1) {
		//			KB[ ne->indices[i] ].discardable = false;
		//		}
		//	}
		//}

		//// if one of the indices is in the discard slot then it is not usable without full information, 
		//// and not discardable in any situation
		//if (ne->indices[0] == 0) {
		//	KB[0].discardable = false;
		//	if (KB[0].perceivedColor == -1) {
		//		KB[0].usable = false;
		//	}

		//	// if the number is less than the top card number then it is discardable
		//	else if (KB[0].perceivedNum < board[ KB[0].perceivedColor ]) {
		//		KB[0].discardable = true;
		//		KB[0].usable = false;
		//	}
		//}

		//delete ne;
	}
}

void Player::updateKB(vector<CardInfo> &kb, Card c, bool discarded)
{
	// update kb
	for (int i=0; i<kb.size(); i++) {
		// if there's only one if the card left, then it's not discardable
		if (remainingCount(kb[i].perceivedNum, kb[i].perceivedColor) == 1) {
			kb[i].discardable = false;
		}
		if (!discarded) {
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
					kb[i].usable = true;
				}
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
		board.push_back(0);
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
	
	if(nFuses == 1) {
		Event* discard = discardCard();
		return discard;
	}
	//If other player's discard slot is in danger and he doesn't know, let them know
	//Else ignore
	//If no hints, skip
	if(checkDiscardSlot(oHand[0])) {
		if(nHints > 0) {
			if(oKB[0].perceivedNum == -1) {
				vector<int> numIndices;
				for(int i = 0; i< oHand.size(); i++) {
					if(oHand[i].number == oHand[0].number) {
						numIndices.push_back(i);
					}
				}

				NumberHintEvent* hint = new NumberHintEvent(numIndices, oHand[0].number);
				for(int i = 0; i < numIndices.size(); i++) {
					oKB[numIndices[i]].perceivedNum = oHand[0].number;
				}
				return hint;

			}
			else if(oKB[0].perceivedColor == -1) {
				vector<int> colorIndices;
				for(int i = 0; i < oHand.size(); i++) {
					if(oHand[i].color == oHand[0].color) {
						colorIndices.push_back(i);
					}
				}
				ColorHintEvent* hint = new ColorHintEvent(colorIndices, oHand[0].color);
				for(int i = 0; i < colorIndices.size(); i++) {
					oKB[colorIndices[i]].perceivedColor = oHand[0].color;
				}
				return hint;
			}
		}
	}

	//If I know the card in my discard slot is in danger
	//Swap it with a safe card
	//If I have no safe cards, ignore
	if(checkDiscardSlot(KB[0])) {
		for(int i = 1; i < KB.size(); i++) {
			if(KB[i].discardable) {
				SwapEvent* swap = new SwapEvent(0,i);
				CardInfo temp = KB[0];
				KB[0] = KB[i];
				KB[i] = temp;
				return swap;
			}
		}
		//Swap valuable card from discard slot
	}
	Event* e = NULL;
	if(nHints > 0) {
		e = crucialPlay(oHand);
	}
	if(e != NULL) {
		return e;
	}
	e = myCrucialPlay();
	if(e != NULL) {
		return e;
	}

	e = discardCard();
	return e;
}

bool Player::checkDiscardSlot(CardInfo k) {
	//If the card is considered discardable, ignore
	if(k.discardable) return false;
	else return true;
}

bool Player::checkDiscardSlot(Card k) {
	//If card in other player's discard is the last of it's kind, and the other player doesn't know about it
	if((remainingCount(k.number, k.color) == 1) && oKB[0].discardable == true) {
		return true;
	}
	return false;
}

Event* Player::crucialPlay(vector<Card> oHand) {
	vector<Card> neededCards;
	vector<Card> validCards;
	vector<int> validCardLocs;
	for(int i = 0; i < board.size(); i++) {
		if(board[i] < 5) {
			Card temp(i,board[i]+1);
			neededCards.push_back(temp);
		}
	}
	for(int i = 0; i < oHand.size(); i++) {
		int val = oHand[i].number;
		int col = oHand[i].color;

		for(int j = 0; j < neededCards.size(); j++) {
			if(neededCards[j].number == val && neededCards[j].color == col) {
				validCards.push_back(oHand[i]);
				validCardLocs.push_back(i);
			}
		}
	}
	int bestColorIndex;
	int bestNumIndex;
	int smallestColor = 6;
	int smallestNum = 6;
	int colorCount = 0;
	int numCount = 0;
	for(int i = 0; i < validCards.size(); i++) {
		for(int j = 0; j < oHand.size(); j++) {
			if(validCards[i].color == oHand[j].color) {
				colorCount++;
			}
			if(validCards[i].number == oHand[j].number) {
				numCount++;
			}
		}
		if(colorCount < smallestColor) {
			smallestColor = colorCount;
			bestColorIndex = i;
		}
		if(numCount < smallestNum) {
			smallestNum = numCount;
			bestNumIndex = i;
		}
		colorCount = 0;
		numCount = 0;
	}
	if(smallestNum == 1) {
		if(oKB[validCardLocs[bestNumIndex]].perceivedColor != -1 && oKB[validCardLocs[bestNumIndex]].perceivedNum != -1) {
			return NULL;
		}
		else if(oKB[validCardLocs[bestNumIndex]].perceivedNum != -1) {
			vector<int> indices;
			for(int i = 0; i < oHand.size(); i++) {
				if(validCards[bestNumIndex].color == oHand[i].color) {
					indices.push_back(i);
				}
			}
			ColorHintEvent* hint = new ColorHintEvent(indices,validCards[bestNumIndex].color);
			for(int i = 0; i < indices.size(); i++) {
				oKB[indices[i]].perceivedColor = validCards[bestNumIndex].color;
				oKB[indices[i]].discardable = false;
			}
			return hint;
		}
		else {
			vector<int> indices;
			indices.push_back(validCardLocs[bestNumIndex]);
			NumberHintEvent* hint = new NumberHintEvent(indices, validCards[bestNumIndex].number);
			oKB[indices[0]].perceivedNum = validCards[bestNumIndex].number;
			oKB[indices[0]].discardable = false;
			oKB[indices[0]].usable = true;
			return hint;
		}
	}
	else if (smallestColor == 1) {
		if(oKB[validCardLocs[bestColorIndex]].perceivedColor != -1 && oKB[validCardLocs[bestColorIndex]].perceivedNum != -1) {
			return NULL;
		}
		else if(oKB[validCardLocs[bestColorIndex]].perceivedColor != -1) {
			vector<int> indices;
			for(int i = 0; i < oHand.size(); i++) {
				if(validCards[bestColorIndex].number == oHand[i].number) {
					indices.push_back(i);
				}
			}
			NumberHintEvent* hint = new NumberHintEvent(indices, validCards[bestColorIndex].number);
			for(int i = 0; i < indices.size(); i++) {
				oKB[indices[i]].perceivedNum = validCards[bestColorIndex].number;
				oKB[indices[i]].discardable = false;
			}
			return hint;
		}
		else {
			vector<int> indices;
			indices.push_back(validCardLocs[bestColorIndex]);
			ColorHintEvent* hint = new ColorHintEvent(indices, validCards[bestColorIndex].color);
			oKB[indices[0]].perceivedColor = validCards[bestColorIndex].color;
			oKB[indices[0]].discardable = false;
			oKB[indices[0]].usable = true;
			return hint;
		}
	}
	else {
		for(int i = 0; i < validCards.size(); i++) {
			if(validCards[i].color != -1 || validCards[i].number != -1) {
				if(oKB[validCardLocs[i]].perceivedColor != -1) {
					vector<int> indices;
					for(int j = 0; j < oHand.size(); j++) {
						if(validCards[i].number == oHand[j].number) {
							indices.push_back(j);
						}
					}
					NumberHintEvent* hint = new NumberHintEvent(indices, validCards[i].number);
					for(int j = 0; j < indices.size(); j++) {
						oKB[indices[j]].perceivedNum = validCards[i].number;
						oKB[indices[j]].discardable = false;
					}
					return hint;
				}
				else if(oKB[validCardLocs[i]].perceivedNum != -1) {
					vector<int> indices;
					for(int j = 0; j < oHand.size(); j++) {
						if(validCards[i].color == oHand[j].color) {
							indices.push_back(j);
						}
					}
					ColorHintEvent* hint = new ColorHintEvent(indices, validCards[i].color);
					for(int j = 0; j < indices.size(); j++) {
						oKB[indices[j]].perceivedColor = validCards[i].color;
						oKB[indices[j]].discardable = false;
					}
					return hint;
				}
			}
		}
	}
	return NULL;
}

Event* Player::myCrucialPlay() {
	for(int i = 0; i < KB.size(); i++) {
		if(KB[i].usable) {
			PlayEvent* play = new PlayEvent(i);
			return play;
		}
	}
	return NULL;
}

Event* Player::discardCard() {
	DiscardEvent* discard = new DiscardEvent(0);
	return discard;
}

int Player::remainingCount(int val, int color) {
	int count = 0;
	int played = 0;

	//Check other player hand
	for(int i = 0; i < oHand.size(); i++) {
		if(oHand[i].number == val && oHand[i].color == color) {
			count++;
		}
	}

	//Check discard
	for(int i = 0; i < discardPile.size(); i++) {
		if(discardPile[i].number == val && discardPile[i].color == color) {
			played++;
		}
	}

	//Check board
	for(int i = 0; i < board.size(); i++) {
		if(color == i && val <= board[i]) {
			played++;
		}
	}
	if(val==1) {
		return 3 - played;
	}
	if(val==2 || val==3 || val==4) {
		return 2 - played;
	}
	if(val==5) {
		return 1 - played;
	}
}

#endif
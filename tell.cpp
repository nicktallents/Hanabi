#include "Player.h"

void Player::tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize)
{
	if (e->getAction() == SWAP) {
		SwapEvent* se = static_cast<SwapEvent*>(e);
		swap(oKB[se->firstPosition], oKB[se->secondPosition]);
		this->oHand = oHand;
		delete se;
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
		}
		else {
			updateKB(KB, pe->c);
			oKB.erase(oKB.begin()+pe->position);
			this->oHand = oHand;
		}
		delete pe;
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
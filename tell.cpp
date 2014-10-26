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
			// update KB
			if (!pe->wasItThisPlayer) {
				for (int i=0; i<KB.size(); i++) {
					// if there's only one if the card left, then it's not discardable
					if (remainingCount(KB[i].perceivedNum, KB[i].perceivedColor) == 1) {
						KB[i].discardable = false;
					}
					// if it's usable, see if it's not usable / discardable
					if (KB[i].usable) {
						// if it's the same card...
						if (KB[i].perceivedColor == pe->c.color && KB[i].perceivedNum == pe->c.number) {
							KB[i].usable = false;
							KB[i].discardable = true;
						}
						// if it's the same color, but we don't know the number...
						else if (KB[i].perceivedColor == pe->c.color && KB[i].perceivedNum == -1) {
							KB[i].usable = false;
						}
						// if it's the same number, but we don't know the color...
						else if (KB[i].perceivedColor == -1 && KB[i].perceivedNum == pe->c.number) {
							KB[i].usable = false;
						}
					}
					// if it's not usable, see if it's usable now
					else {
						if (KB[i].perceivedColor == pe->c.color && KB[i].perceivedNum - 1 == pe->c.number) {
							KB[i].usable == true;
						}
					}
				}
			}
		}
		else {
			this->discardPile.push_back(pe->c);
			this->nFuses = fuses;
			this->nHints = hints;
		}
		// remove the card from either the player's KB or partner's KB and hand
		if (pe->wasItThisPlayer) {
			KB.erase(KB.begin()+pe->position);
		}
		else {
			oKB.erase(oKB.begin()+pe->position);
			this->oHand = oHand;
		}
		delete pe;
	}
}
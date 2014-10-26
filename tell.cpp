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
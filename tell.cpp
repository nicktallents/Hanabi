#include "Player.h"

void Player::tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize)
{
	if (e->getAction() == SWAP) {
		SwapEvent* se = static_cast<SwapEvent*>(e);
		swap(oKB[se->firstPosition], oKB[se->secondPosition]);
		this->oHand = oHand;
	}

	else if (e->getAction() == PLAY) {
		PlayEvent* pe = static_cast<PlayEvent*>(e);
		
		if (pe->wasItThisPlayer) {
			if (pe->legal) {
				;
			}
			else {
				;
			}
		}
		else {
			;
		}
	}
}
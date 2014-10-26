#include "Player.h"

void Player::tell(Event* e, vector<int> board, int hints, int fuses, vector<Card> oHand, int deckSize)
{
	if (e->getAction() == SWAP) {
		int index1 = -1;
		int index2 = -1;
		// find cards that are out of place
		for (int i=0; i<oHand.size(); i++) {
			if (this->oHand[i] != oHand[i]) {
				if (index1 == -1) {
					index1 = i;
					continue;
				}
				else if (index2 == -1) {
					index2 = i;
				}
			}
		}
		// if swapped like for like ----- this method only works if there are only 2 like cards. Problems if there are 3
		if (index1 == -1) {
			// find the similar cards
			bool found = false;
			for (int i=0; i<oHand.size()-1; i++) {
				for (int j=(i+1); j<oHand.size(); j++) {
					// if cards are the same
					if (this->oHand[i] == oHand[j]) {
						index1 = i;
						index2 = j;
						found = true;
						break;
					}
				}
				if (found)
					break;
			}
		}
		swap(oKB[index1], oKB[index2]);
		this->oHand = oHand;
	}
}
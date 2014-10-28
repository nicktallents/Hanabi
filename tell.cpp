#include "Player.h"

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
#include "GameConstants.h"
#include "Card.h"
#include "Events.h"
#include "Player.h"
#include <vector>

using std::vector;

#ifndef FULL_PLAYER_H
#define FULL_PLAYER_H

struct FullPlayer
{
	FullPlayer();
	FullPlayer(const FullPlayer& f);
	FullPlayer(Player& play, vector<Card> h);
	Player p;
	vector<Card> hand;
};

FullPlayer::FullPlayer()
{
}

FullPlayer::FullPlayer(const FullPlayer& f)
{
	p = f.p;
	hand = f.hand;
}

FullPlayer::FullPlayer(Player& play, vector<Card> h): p(play)
{
	hand = h;
}

#endif
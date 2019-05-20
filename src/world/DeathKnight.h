#ifndef _DEATHKNIGHT_H
#define _DEATHKNIGHT_H

class DeathKnight : public Player
{
public:
	DeathKnight(uint32 guid) : Player(guid) {}

	bool IsDeathKnight() { return true; }
};

#endif // _DEATHKNIGHT_H
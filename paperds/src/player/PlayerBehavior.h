#pragma once

class Player;

class PlayerBehavior
{
protected:
	Player* _player;

public:
	PlayerBehavior(Player* player)
		: _player(player)
	{}

	virtual void Update() = 0;
};
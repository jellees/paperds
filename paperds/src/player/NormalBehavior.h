#pragma once

#include "PlayerBehavior.h"

class NormalBehavior : public PlayerBehavior
{
public:
	NormalBehavior(Player* player);
	~NormalBehavior();

	void Update();
};


#include "common.h"
#include "Player.h"
#include "NormalBehavior.h"



NormalBehavior::NormalBehavior(Player* player)
	: PlayerBehavior(player)
{
}


NormalBehavior::~NormalBehavior()
{
}


void NormalBehavior::Update()
{
	u16 pad = PAD_Read();
	if (pad & (PAD_KEY_RIGHT | PAD_KEY_LEFT | PAD_KEY_UP | PAD_KEY_DOWN))
	{
		VecFx32 direction = { 0, 0, 0 };

		if (pad & PAD_KEY_RIGHT)
			direction.x = FX32_ONE;
		else if (pad & PAD_KEY_LEFT)
			direction.x = -FX32_ONE;

		if (pad & PAD_KEY_UP)
			direction.z = -FX32_ONE;
		else if (pad & PAD_KEY_DOWN)
			direction.z = FX32_ONE;

		VEC_Normalize(&direction, &direction);
		_player->SetDirection(&direction);

		if (pad & (PAD_KEY_UP | PAD_KEY_DOWN))
			_player->Walk(FX32_CONST(0.75));
		else
			_player->Walk(FX32_CONST(1));
	}
}

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
	if (gKeys & (PAD_KEY_RIGHT | PAD_KEY_LEFT | PAD_KEY_UP | PAD_KEY_DOWN))
	{
		// Calculate direciton.
		VecFx32 direction = { 0, 0, 0 };

		if (gKeys & PAD_KEY_RIGHT)
			direction.x = FX32_ONE;
		else if (gKeys & PAD_KEY_LEFT)
			direction.x = -FX32_ONE;

		if (gKeys & PAD_KEY_UP)
			direction.z = -FX32_ONE;
		else if (gKeys & PAD_KEY_DOWN)
			direction.z = FX32_ONE;

		VEC_Normalize(&direction, &direction);
		_player->SetDirection(&direction);

		// Calculate the speed.
		fx32 speed;
		if (gKeys & (PAD_KEY_UP | PAD_KEY_DOWN))
			speed = FX32_CONST(1.5);
		else
			speed = FX32_CONST(3);

		_player->SetMaxSpeed(speed);
		_player->Walk(FX32_CONST(1));
	}


	// Jump stuff.
	if (gKeysDown & (PAD_BUTTON_A) && _player->OnGround())
	{
		VecFx32 jump = { 0, FX32_CONST(10) ,0 };
		_player->AddForce(&jump);
	}
}

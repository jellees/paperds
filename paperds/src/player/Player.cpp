#include "common.h"
#include "PlayerBehavior.h"
#include "NormalBehavior.h"
#include "Player.h"



Player::Player()
{
	VEC_Set(&_position, 0, 0, 0);
	VEC_Set(&_prevPosition, 0, 0, 0);
	VEC_Set(&_scale, FX32_CONST(2), FX32_CONST(2), FX32_CONST(2));
	VEC_Set(&_direction, 0, 0, FX32_ONE);

	_horizontalVelocity = 0;
	_verticalVelocity = 0;
	_gravity = 0;
	
	_normalBehavior = new NormalBehavior(this);
	_currentBehavior = _normalBehavior;

	_modelResource = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/grandstar.nsbmd", NULL, FALSE);
	NNS_G3dResDefaultSetup(_modelResource);
	NNSG3dResFileHeader* resourceTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/grandstar.nsbtx", NULL, TRUE);
	NNS_G3dResDefaultSetup(resourceTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(_modelResource), NNS_G3dGetTex(resourceTextures));
	NNS_G3dRenderObjInit(&_modelRender, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(_modelResource), 0));
	NNS_FndFreeToExpHeap(gHeapHandle, resourceTextures);
}


Player::~Player()
{
	NNS_FndFreeToExpHeap(gHeapHandle, _modelResource);
}


void Player::Update()
{
	_prevPosition = _position;

	_currentBehavior->Update();

	VecFx32 velocity;
	VEC_MulByFx32(&_direction, _horizontalVelocity, &velocity);
	velocity.y = _verticalVelocity;
	_verticalVelocity -= _gravity;
	VEC_Add(&_position, &velocity, &_position);
}


void Player::Render()
{
	VecFx32 position;
	VEC_ToRenderSpace(&_position, &position);

	NNS_G3dGePushMtx();
	{
		NNS_G3dGeTranslateVec(&position);
		NNS_G3dGeScaleVec(&_scale);
		NNS_G3dDraw(&_modelRender);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGlbFlush();
}

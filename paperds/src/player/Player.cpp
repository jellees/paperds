#include "common.h"
#include "PlayerBehavior.h"
#include "NormalBehavior.h"
#include "Player.h"



void Player::AddFriction()
{
	VecFx32 friction;
	VEC_Normalize(&_velocity, &friction);
	VEC_MulByFx32(&friction, _friction, &friction);
	// Fix imprecissions.
	friction.x = _velocity.x > _friction && _velocity.x < -_friction ? -_velocity.x : friction.x;
	friction.y = _velocity.y > _friction && _velocity.y < -_friction ? -_velocity.y : friction.y;
	friction.z = _velocity.z > _friction && _velocity.z < -_friction ? -_velocity.z : friction.z;
	AddForce(&friction);
}


void Player::AddDrag()
{
	VecFx32 drag;
	VEC_Normalize(&_velocity, &drag);
	fx32 speed = VEC_Mag(&_velocity);
	speed = FX_Mul(speed, speed);
	VEC_MulByFx32(&drag, FX_Mul(_drag, speed), &drag);
	AddForce(&drag);
}


Player::Player()
{
	VEC_Set(&_position, 0, 0, 0);
	VEC_Set(&_prevPosition, 0, 0, 0);
	VEC_Set(&_scale, FX32_CONST(1), FX32_CONST(1), FX32_CONST(1));
	VEC_Set(&_direction, FX32_ONE, 0, 0);
	VEC_Set(&_acceleration, 0, 0, 0);
	VEC_Set(&_velocity, 0, 0, 0);

	_gravity = 0;
	_friction = FX32_CONST(-0.5);
	_drag = FX32_CONST(-0.1);
	_grip = FX32_CONST(1);
	
	_normalBehavior = new NormalBehavior(this);
	_currentBehavior = _normalBehavior;

	_modelResource = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario_prot.nsbmd", NULL, FALSE);
	NNS_G3dResDefaultSetup(_modelResource);
	NNSG3dResFileHeader* resourceTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario_prot.nsbtx", NULL, TRUE);
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

	// Add the gravity as force.
	VecFx32 gravity = { 0, -_gravity, 0 };
	AddForce(&gravity);
	
	// Add other forces.
	AddFriction();
	AddDrag();

	// Add acceleration to velocity and velocity to position.
	VEC_Add(&_velocity, &_acceleration, &_velocity);
	VEC_Add(&_position, &_velocity, &_position);

	//NOCASH_Printf("acceleration: %X, %X,%X", _acceleration.x, _acceleration.y, _acceleration.z);
	NOCASH_Printf("_velocity: %X, %X,%X", _velocity.x, _velocity.y, _velocity.z);

	// Reset the acceleration because each frame is a new time in space.
	VEC_Set(&_acceleration, 0, 0, 0);
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

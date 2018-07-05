#include "common.h"
#include "Test.h"
#include "PlayerBehavior.h"
#include "NormalBehavior.h"
#include "animation/AnimationManager.h"
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
	//VEC_Set(&_position, FX32_CONST(1000), FX32_CONST(1200), 0);
	VEC_Set(&_position, FX32_CONST(100), FX32_CONST(100), 0);
	VEC_Set(&_prevPosition, 0, 0, 0);
	VEC_Set(&_scale, FX32_CONST(1), FX32_CONST(1), FX32_CONST(1));
	VEC_Set(&_direction, FX32_ONE, 0, 0);
	VEC_Set(&_acceleration, 0, 0, 0);
	VEC_Set(&_velocity, 0, 0, 0);

	_maxSpeed = FX32_CONST(3);
	_maxGravity = FX32_CONST(40);

	_gravity = FX32_CONST(0.5);
	_friction = FX32_CONST(-0.5);
	_drag = FX32_CONST(-0.1);
	_grip = FX32_CONST(1);
	
	_normalBehavior = new NormalBehavior(this);
	_currentBehavior = _normalBehavior;

	_modelResource = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario/mario.nsbmd", NULL, FALSE);
	NNS_G3dResDefaultSetup(_modelResource);
	NNSG3dResFileHeader* resourceTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario/mario.nsbtx", NULL, TRUE);
	NNS_G3dResDefaultSetup(resourceTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(_modelResource), NNS_G3dGetTex(resourceTextures));
	NNS_G3dRenderObjInit(&_modelRender, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(_modelResource), 0));
	NNS_FndFreeToExpHeap(gHeapHandle, resourceTextures);

	_collider = new SphereCollider(FX32_CONST(10));

	_paper = Paper();

	NNSG3dResFileHeader** animationResources = new NNSG3dResFileHeader*[2];
	animationResources[0] = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario/walk.nsbca", 0, false);
	animationResources[1] = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/mario/idle.nsbca", 0, false);
	_animationManager = new AnimationManager(2, animationResources, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(_modelResource), 0));
	_animationManager->SetRender(&_modelRender);
	_animationManager->SetAnimation(1, FX32_CONST(0.5));
}


Player::~Player()
{
	NNS_FndFreeToExpHeap(gHeapHandle, _modelResource);
	
	delete _normalBehavior;

	delete _collider;
}


void Player::Update(Test* test)
{
	_prevPosition = _position;

	_currentBehavior->Update();

	// Add the gravity as force.
	if (!_collider->mResponse.floor || _velocity.x || _velocity.z)
	{
		VecFx32 gravity = { 0, -_gravity, 0 };
		AddForce(&gravity);
	}

	// Add friction as force.
	if (_collider->mResponse.floor)
	{
		AddFriction();
	}

	// Add acceleration to velocity and velocity to position.
	VEC_Add(&_velocity, &_acceleration, &_velocity);
	_velocity.x = MATH_CLAMP(_velocity.x, -_maxSpeed, _maxSpeed);
	_velocity.y = MATH_CLAMP(_velocity.y, -_maxGravity, _maxGravity);
	_velocity.z = MATH_CLAMP(_velocity.z, -_maxSpeed, _maxSpeed);
	VEC_Add(&_position, &_velocity, &_position);

	// Collide with the main mesh.
	_collider->SetPosition(&_prevPosition, &_position);
	test->_meshCollider->Collide(_collider);
	_collider->GetPosition(&_prevPosition, &_position);

	//NOCASH_Printf("acceleration: %X, %X, %X", _acceleration.x, _acceleration.y, _acceleration.z);
	NOCASH_Printf("_velocity: %X, %X, %X", _velocity.x / FX32_ONE, _velocity.y / FX32_ONE, _velocity.z / FX32_ONE);

	// Reset the acceleration because each frame is a new time in space.
	VEC_Set(&_acceleration, 0, 0, 0);

	// Set y velocity to 0 if collision is found.
	if (_collider->mResponse.floor && _velocity.y < 0)
		_velocity.y = 0;

	// Update paper mechanics.
	_paper.Update();

	// Delete this later.
	if (gKeysDown & PAD_KEY_LEFT)
		_paper.Flip(false);
	else if (gKeysDown & PAD_KEY_RIGHT)
		_paper.Flip(true);


	// Delete this later.
	if(_velocity.x != 0 || _velocity.z != 0)
		_animationManager->SetAnimation(0, FX32_CONST(2));
	else
		_animationManager->SetAnimation(1, FX32_CONST(0.5));
	_animationManager->Update();
}

void Player::Render()
{
	VecFx32 position = { 0, FX32_CONST(-10), 0 };
	VEC_Add(&_position, &position, &position);
	VEC_ToRenderSpace(&position, &position);

	NNS_G3dGePushMtx();
	{
		NNS_G3dGeTranslateVec(&position);

		_paper.GetScale(&_scale);
		NNS_G3dGeScaleVec(&_scale);
		
		MtxFx33 rotation;
		_paper.GetMatrix(&rotation);
		NNS_G3dGeMultMtx33(&rotation);

		NNS_G3dDraw(&_modelRender);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGlbFlush();
}

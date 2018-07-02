#pragma once

#include "collider/SphereCollider.h"
#include "Paper.h"

#define PLAYER_VEL_LIMIT FX32_CONST(1)

class PlayerBehavior;
class Test;
class AnimationManager;

class Player
{
private:
	VecFx32 _acceleration;
	VecFx32 _velocity;
	VecFx32 _position;
	VecFx32 _prevPosition;

	VecFx32 _scale;
	VecFx32 _direction;

	fx32 _maxSpeed;
	fx32 _maxGravity;

	fx32 _gravity;
	fx32 _friction;
	fx32 _drag;
	fx32 _grip;

	PlayerBehavior* _normalBehavior;
	PlayerBehavior* _currentBehavior;

	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

	SphereCollider* _collider;

	Paper _paper;
	AnimationManager* _animationManager;

	void AddFriction();
	void AddDrag();

public:
	Player();
	~Player();

	void Update(Test* test);
	void Render();

	void GetPosition(VecFx32* position)
	{
		*position = _position;
	}

	void GetVelocity(VecFx32* velocity)
	{
		*velocity = _velocity;
	}

	void SetDirection(VecFx32* direction)
	{
		_direction = *direction;
	}

	void SetGravity(fx32 gravity)
	{
		_gravity = gravity;
	}

	void SetMaxSpeed(fx32 speed)
	{
		_maxSpeed = speed;
	}

	void AddForce(VecFx32* force)
	{
		VEC_Add(&_acceleration, force, &_acceleration);
	}

	void AddForceOnDirection(fx32 force)
	{
		VecFx32 directionalForce;
		VEC_MulByFx32(&_direction, force, &directionalForce);
		VEC_Add(&_acceleration, &directionalForce, &_acceleration);
	}

	void Walk(fx32 speed)
	{
		AddForceOnDirection(FX_Mul(speed, _grip));
	}

	bool OnGround()
	{
		return _collider->mResponse.floor;
	}
};


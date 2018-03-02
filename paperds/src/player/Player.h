#pragma once

class PlayerBehavior;

class Player
{
private:
	VecFx32 _acceleration;
	VecFx32 _velocity;
	VecFx32 _position;
	VecFx32 _prevPosition;

	VecFx32 _scale;
	VecFx32 _direction;

	fx32 _gravity;
	fx32 _friction;

	PlayerBehavior* _normalBehavior;
	PlayerBehavior* _currentBehavior;

	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

public:
	Player();
	~Player();

	void Update();
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
};


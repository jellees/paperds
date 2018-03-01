#pragma once

class PlayerBehavior;

class Player
{
private:
	VecFx32 _position;
	VecFx32 _prevPosition;
	VecFx32 _scale;
	VecFx32 _direction;

	fx32 _horizontalVelocity;
	fx32 _verticalVelocity;

	fx32 _gravity;

	PlayerBehavior* _normalBehavior;
	PlayerBehavior* _currentBehavior;

	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

public:
	Player();
	~Player();

	void Update();
	void Render();

	void SetPosition(VecFx32* position)
	{
		_position = *position;
	}

	void GetPosition(VecFx32* position)
	{
		*position = _position;
	}

	void SetHorizontalVelocity(fx32 velocity)
	{
		_horizontalVelocity = velocity;
	}

	void SetVerticalVelocity(fx32 velocity)
	{
		_verticalVelocity = velocity;
	}

	void SetGravity(fx32 gravity)
	{
		_gravity = gravity;
	}
};


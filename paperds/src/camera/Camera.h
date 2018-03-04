#pragma once

class CameraBehavior;

class Camera
{
private:
	// The position the camera is looking from.
	VecFx32 _position;
	// The up vector.
	VecFx32 _up;
	// The position the camera is looking at.
	VecFx32 _at;

	// A 3d vector to use as target to follow.
	VecFx32 _target;

	// All behaviors and the current active camera;
	CameraBehavior* _standardCamera;
	CameraBehavior* _currentCamera;

	void Init(VecFx32* position);

public:
	enum Type
	{
		CAMERA_STANDARD
	};

	Camera();
	Camera(VecFx32* position);
	~Camera();
	
	// Update the camera.
	void Update();

	// Apply the camera matrix.
	void Apply();

	// Sets the type camera as the active camera.
	void SetType(Type type);

	// Sets the target the camera is going to follow.
	void SetTarget(VecFx32* target)
	{
		_target = *target;
	}

	// Gets the vector 3d of the position that the camera follows.
	void GetTarget(VecFx32* target)
	{
		*target = _target;
	}

	// Sets the position of the camera.
	void SetPosition(VecFx32* position)
	{
		_position = *position;
	}

	// Gets the position of the camera.
	void GetPosition(VecFx32* position)
	{
		*position = _position;
	}

	// Sets the position the camera is looking at.
	void SetAt(VecFx32* at)
	{
		_at = *at;
	}

	// Gets the position the camera is looking at.
	void GetAt(VecFx32* at)
	{
		*at = _at;
	}
};


#pragma once

typedef Camera;

class CameraBehavior
{
protected:
	Camera* _camera;

public:
	CameraBehavior(Camera* camera)
		: _camera(camera)
	{ }

	virtual void Update() = 0;
};
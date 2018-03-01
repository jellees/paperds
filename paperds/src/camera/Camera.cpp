#include "common.h"
#include "StandardCamera.h"
#include "Camera.h"




void Camera::Init(VecFx32 * position)
{
	_position = *position;
	VEC_Set(&_up, 0, FX32_ONE, 0);
	VEC_Set(&_at, 0, 0, 0);

	_standardCamera = new StandardCamera(this);
	_currentCamera = _standardCamera;
}


Camera::Camera()
{
	VecFx32 position;
	VEC_Set(&position, 0, 0, FX32_ONE);
	Init(&position);
}


Camera::Camera(VecFx32* position)
{
	Init(position);
}


Camera::~Camera()
{
}


void Camera::Update()
{
	_currentCamera->Update();
}


void Camera::Apply()
{
	VecFx32 up = _up;

	VecFx32 pos = _position;
	pos.x = (pos.x + 8) >> 4;
	pos.y = (pos.y + 8) >> 4;
	pos.z = (pos.z + 8) >> 4;

	VecFx32 target = _at;
	target.x = (target.x + 8) >> 4;
	target.y = (target.y + 8) >> 4;
	target.z = (target.z + 8) >> 4;

	NNS_G3dGlbLookAt(&pos, &up, &target);
}


void Camera::SetType(Type type)
{
	switch (type)
	{
	case Camera::CAMERA_STANDARD:
	default:
		_currentCamera = _standardCamera;
	}
}

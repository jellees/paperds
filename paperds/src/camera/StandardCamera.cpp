#include "common.h"
#include "Camera.h"
#include "StandardCamera.h"



StandardCamera::StandardCamera(Camera* camera) 
	: CameraBehavior(camera)
{
}


StandardCamera::~StandardCamera()
{
}


void StandardCamera::Update()
{
	VecFx32 target;
	_camera->GetTarget(&target);
	target.y += FX32_CONST(30);
	_camera->SetAt(&target);
	target.z += FX32_CONST(100);
	target.y += FX32_CONST(20);
	_camera->SetPosition(&target);
}

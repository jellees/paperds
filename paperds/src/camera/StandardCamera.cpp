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
	VecFx32 target, at;
	_camera->GetTarget(&target);
	_camera->GetAt(&at);

	target.y += FX32_CONST(30);
	target.y = at.y + FX_Mul(target.y - at.y, FX32_CONST(0.1));
	_camera->SetAt(&target);

	target.z += FX32_CONST(125);
	target.y += FX32_CONST(20);
	_camera->SetPosition(&target);
}

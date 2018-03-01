#pragma once

#include "CameraBehavior.h"

class StandardCamera : public CameraBehavior
{
public:
	StandardCamera(Camera*);
	~StandardCamera();

	void Update();
};


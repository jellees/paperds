#pragma once

class Player;
class Camera;

class Test
{
private:
	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

	/*VecFx32 _up;
	VecFx32 _position;
	VecFx32 _at;*/

	Player* _mario;

	Camera* _camera;

public:
	Test();
	~Test();

	void Update();
	void Render();
	void* OpenFile(char*);
};


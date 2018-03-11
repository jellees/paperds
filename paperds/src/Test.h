#pragma once

class Player;
class Camera;
class Collider;

class Test
{
private:
	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

	Player* _mario;
	Camera* _camera;

public:
	Collider * _meshCollider;

	Test();
	~Test();

	void Update();
	void Render();
};


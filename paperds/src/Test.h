#pragma once

class Player;

class Test
{
private:
	NNSG3dResFileHeader* _modelResource;
	NNSG3dRenderObj _modelRender;

	VecFx32 _up;
	VecFx32 _position;
	VecFx32 _at;

	Player* _mario;

public:
	Test();
	~Test();

	void Update();
	void Render();
	void* OpenFile(char*);
};


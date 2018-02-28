#pragma once
class Test
{
private:
	NNSG3dResFileHeader* mModelResource;
	NNSG3dRenderObj mModelRender;

	VecFx32 mUp;
	VecFx32 mPosition;
	VecFx32 mAt;

public:
	Test();
	~Test();

	void Update();
	void Render();
	void* OpenFile(char*);
};


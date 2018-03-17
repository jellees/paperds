#pragma once

class Paper
{
private:
	bool _isRight;
	char _isRotating;
	char _progression;
	fx32 _rotation;
	MtxFx33 _matrix;
	VecFx32 _scale;

public:
	Paper();

	void Update();
	void Flip(bool toRight);

	void GetMatrix(MtxFx33* matrix)
	{
		*matrix = _matrix;
	}

	void GetScale(VecFx32* scale)
	{
		*scale = _scale;
	}
};
#include "common.h"
#include "Paper.h"


#define PAPER_SPEED FX32_CONST(15)


Paper::Paper()
{
	_isRight = true;
	_isRotating = -1;
	_rotation = 0;
	VEC_Set(&_scale, FX32_ONE, FX32_ONE, FX32_ONE);
	MTX_RotY33(&_matrix, FX_SinIdx(FX_DEG_TO_IDX(_rotation)), FX_CosIdx(FX_DEG_TO_IDX(_rotation)));
}


void Paper::Update()
{
	if (_isRotating != -1)
	{
		if (_isRotating == 0)
		{
			_scale.x = -FX32_ONE;
		}
		else if (_isRotating == 1)
		{
			_scale.x = FX32_ONE;
		}

		if (_progression == 0)
		{
			_rotation += PAPER_SPEED;

			if (_rotation > FX32_CONST(90))
			{
				_progression++;
				_rotation = FX32_CONST(90);
				_isRotating = !_isRotating;
			}
		}
		else if (_progression == 1)
		{
			_rotation -= PAPER_SPEED;

			if (_rotation < FX32_CONST(0))
			{
				_isRotating = -1;
				_rotation = FX32_CONST(0);
			}
		}

		MTX_RotY33(&_matrix, FX_SinIdx(FX_DEG_TO_IDX(_rotation)), FX_CosIdx(FX_DEG_TO_IDX(_rotation)));
	}
}


void Paper::Flip(bool toRight)
{
	if (_isRight == toRight)
		return;

	_isRight = toRight;
	_isRotating = toRight;
	_progression = 0;
}

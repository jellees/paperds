#pragma once

#include "common.h"

struct Cube3d
{
	fx32 x1, y1, z1;
	fx32 x2, y2, z2;

	inline void initialize(VecFx32 *point1, VecFx32 *point2)
	{
		x1 = point1->x;
		y1 = point1->y;
		z1 = point1->z;
		x2 = point2->x;
		y2 = point2->y;
		z2 = point2->z;
	}

	inline void setPoint(VecFx32 *point)
	{
		x1 = point->x;
		y1 = point->y;
		z1 = point->z;
	}

	inline void setLength(fx32 x, fx32 y, fx32 z)
	{
		x2 = x1 + x;
		y2 = y1 + y;
		z2 = z1 + z;
	}

	inline void setEqualOffset(fx32 offset)
	{
		x1 = -offset;
		y1 = -offset;
		z1 = -offset;
		x2 = offset;
		y2 = offset;
		z2 = offset;
	}

	inline bool inside(VecFx32 *vec)
	{
		return vec->x >= x1 && vec->x <= x2 && vec->y >= y1 && vec->y <= y2 && vec->z >= z1 && vec->z <= z2;
	}

	inline bool isZero()
	{
		return !x1 && !x2 && !y1 && !y2 && !z1 && !z2;
	}
};
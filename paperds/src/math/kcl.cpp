#include "common.h"
#include "kcl.h"

#include <nitro/itcm_begin.h>

static void sub_1FFF434(fx32 dot1, fx32 dot2, fx32 dot3, VecFx32* result, VecFx32* a, VecFx32* b, s64* resultLenSquare)
{
	fx32 aFactor = FX_Div(FX_Mul(dot1, dot2) - dot3, FX_Mul(dot1, dot1) - FX32_ONE);// a = (d1*d2-d3)/(d1^2-1)
	fx32 bFactor = dot2 - FX_Mul(aFactor, dot1);// b = (d1*d3-d2)/(d1^2-1)
	result->x = FX_Mul(aFactor, a->x) + FX_Mul(bFactor, b->x);
	result->y = FX_Mul(aFactor, a->y) + FX_Mul(bFactor, b->y);
	result->z = FX_Mul(aFactor, a->z) + FX_Mul(bFactor, b->z);
	*resultLenSquare = (s64)result->z * (s64)result->z + (s64)result->x * (s64)result->x + (s64)result->y * (s64)result->y;
}

int kcl_detect_collision(kcl_header* pKcl, VecFx32* curPos, VecFx32* prevPos, fx32 radius, kcl_response* response)
{
	// Setup response.
	response->floor = 0;
	response->wall = 0;
	response->ceiling = 0;
	response->edge = 0;
	response->water = 0;
	
	// Check whether the current position is inside the mesh.
	int xDiff = (curPos->x - pKcl->octreeOrigin.x) >> 12;
	if (xDiff & pKcl->xMask)
		return 0;
	
	int yDiff = (curPos->y - pKcl->octreeOrigin.y) >> 12;
	if (yDiff & pKcl->yMask)
		return 0;
	
	int zDiff = (curPos->z - pKcl->octreeOrigin.z) >> 12;
	if (zDiff & pKcl->zMask)
		return 0;

	uint32_t shift = pKcl->coordShift;
	uint32_t* octreeRoot = (uint32_t*)(((uint8_t*)pKcl) + pKcl->octreeOffset);
	uint32_t curNode = *(octreeRoot + (((zDiff >> shift) << pKcl->zShift) | ((yDiff >> shift) << pKcl->yShift) | (xDiff >> shift)));
	
	// Loop through the tree to find the corresponding leaf.
	while (!(curNode >> 31))
	{
		octreeRoot = (uint32_t*)(((uint8_t*)octreeRoot) + curNode);
		shift--;
		curNode = *(octreeRoot + (((xDiff >> shift) & 1) | (4 * ((zDiff >> shift) & 1)) | (2 * ((yDiff >> shift) & 1))));
	}
	
	uint16_t* indices = (uint16_t*)((uint8_t*)octreeRoot + (curNode & 0x7FFFFFFF));
	VecFx32* vertices = (VecFx32*)(((uint8_t*)pKcl) + pKcl->vertexOffset);
	VecFx16* normals = (VecFx16*)(((uint8_t*)pKcl) + pKcl->normalsOffset);
	kcl_plane* planes = (kcl_plane*)(((uint8_t*)pKcl) + pKcl->planesOffset);

	// Skip first index.
	indices++;
	
	VecFx32 diffMax = { 0, 0, 0 };
	VecFx32 diffMin = { 0, 0, 0 };
	VecFx32 diffWallMax = { 0, 0, 0 };
	VecFx32 diffWallMin = { 0, 0, 0 };
	fx32 maxFloorLen = -1;
	VecFx32 floorDir = { 0, 0, 0 };
	fx32 maxWallLen = -1;
	VecFx32 wallDir = { 0, 0, 0 };
	
	int succeeded = 0;
	
	while (*indices)
	{
		kcl_plane* curPlane = &planes[*indices++];

		// Instead of testing each vertex against the real position. We move the 
		// real position into the local space of the triangle.
		VecFx32 localPos;
		VEC_Subtract(curPos, &vertices[curPlane->vertexIdx], &localPos);

		// -------------------------------------------------------
		// Test whether localPos + radius is inside the triangle
		// -------------------------------------------------------

		VecFx32 normalC;//from A to BC
		normalC.x = (fx32)normals[curPlane->normalCIdx].x;
		normalC.y = (fx32)normals[curPlane->normalCIdx].y;
		normalC.z = (fx32)normals[curPlane->normalCIdx].z;
		fx32 dotC = VEC_DotProduct(&localPos, &normalC);
		if (dotC < -radius)
			continue;
		if (dotC - curPlane->length >= radius)
			continue;

		VecFx32 normalA;//from B to AC
		normalA.x = (fx32)normals[curPlane->normalAIdx].x;
		normalA.y = (fx32)normals[curPlane->normalAIdx].y;
		normalA.z = (fx32)normals[curPlane->normalAIdx].z;
		fx32 dotA = VEC_DotProduct(&localPos, &normalA);
		if (dotA >= radius)
			continue;

		VecFx32 normalB;//from C to AB
		normalB.x = (fx32)normals[curPlane->normalBIdx].x;
		normalB.y = (fx32)normals[curPlane->normalBIdx].y;
		normalB.z = (fx32)normals[curPlane->normalBIdx].z;
		fx32 dotB = VEC_DotProduct(&localPos, &normalB);
		if (dotB >= radius)
			continue;

		// -------------------------------------------------------
		// Test whether localPos + radius is touching the face
		// -------------------------------------------------------

		VecFx32 normal;
		normal.x = (fx32)normals[curPlane->normalIdx].x;
		normal.y = (fx32)normals[curPlane->normalIdx].y;
		normal.z = (fx32)normals[curPlane->normalIdx].z;
		fx32 dotN = VEC_DotProduct(&localPos, &normal);
		if (radius - dotN < 0 || radius - dotN >(30 * FX32_ONE))
			continue;

		// You are touching the face. This variable holds how far inside
		// the face you are.
		int length = radius - dotN;

		s64 squaredist;

		if (dotA <= dotB && dotB <= dotC - curPlane->length && dotC - curPlane->length > 0)
		{
			if (dotN < 0)
			{
				if (!prevPos)
					continue;
				VecFx32 localPrevPos;
				VEC_Subtract(prevPos, &vertices[curPlane->vertexIdx], &localPrevPos);
				if (VEC_DotProduct(&localPrevPos, &normal) < 0)
					continue;
				if (VEC_DotProduct(&localPrevPos, &normalC) > curPlane->length)
					continue;
			}
			fx32 dotCB = VEC_DotProduct(&normalC, &normalB);
			if (FX_Mul(dotCB, dotC - curPlane->length)  > dotB)
			{
				squaredist = ((s64)radius) * ((s64)radius) - ((s64)(dotC - curPlane->length)) * ((s64)(dotC - curPlane->length));
			}
			else
			{
				sub_1FFF434(dotCB, dotC - curPlane->length, dotB, &localPos, &normalB, &normalC, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
		}
		else if (dotA <= dotB && dotB > 0)
		{
			if (dotN < 0)
			{
				if (!prevPos)
					continue;
				VecFx32 localPrevPos;
				VEC_Subtract(prevPos, &vertices[curPlane->vertexIdx], &localPrevPos);
				if (VEC_DotProduct(&localPrevPos, &normal) < 0 || VEC_DotProduct(&localPrevPos, &normalB) > 0)
					continue;
			}
			fx32 dotCB = VEC_DotProduct(&normalB, &normalC);
			fx32 dotBA = VEC_DotProduct(&normalB, &normalA);
			if (dotC - curPlane->length > dotA && FX_Mul(dotCB, dotB) <= dotC - curPlane->length)
			{
				sub_1FFF434(dotCB, dotC - curPlane->length, dotB, &localPos, &normalB, &normalC, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
			else if (dotC - curPlane->length > dotA || FX_Mul(dotBA, dotB) > dotA)
			{
				squaredist = ((s64)radius) * ((s64)radius) - ((s64)dotB) * ((s64)dotB);
			}
			else
			{
				sub_1FFF434(dotBA, dotB, dotA, &localPos, &normalA, &normalB, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
		}
		else if (dotA > dotB && dotA <= dotC - curPlane->length > 0 && dotC - curPlane->length > 0)
		{
			if (dotN < 0)
			{
				if (!prevPos)
					continue;
				VecFx32 localPrevPos;
				VEC_Subtract(prevPos, &vertices[curPlane->vertexIdx], &localPrevPos);
				if (VEC_DotProduct(&localPrevPos, &normal) < 0)
					continue;
				if (VEC_DotProduct(&localPrevPos, &normalC) > curPlane->length)
					continue;
			}
			fx32 dotCA = VEC_DotProduct(&normalC, &normalA);
			fx32 dotCB = VEC_DotProduct(&normalC, &normalB);
			if (FX_Mul(dotCA, dotC - curPlane->length) > dotA || FX_Mul(dotCB, dotC - curPlane->length) > dotB)
			{
				squaredist = ((s64)radius) * ((s64)radius) - ((s64)(dotC - curPlane->length)) * ((s64)(dotC - curPlane->length));
			}
			else
			{
				sub_1FFF434(dotCB, dotC - curPlane->length, dotB, &localPos, &normalB, &normalC, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
		}
		else if (dotA > dotB && dotA > 0)
		{
			if (dotN < 0)
			{
				if (!prevPos)
					continue;
				VecFx32 localPrevPos;
				VEC_Subtract(prevPos, &vertices[curPlane->vertexIdx], &localPrevPos);
				if (VEC_DotProduct(&localPrevPos, &normal) < 0 || VEC_DotProduct(&localPrevPos, &normalA) > 0)
					continue;
			}
			fx32 dotCA = VEC_DotProduct(&normalA, &normalC);
			fx32 dotBA = VEC_DotProduct(&normalA, &normalB);
			if (dotB <= dotC - curPlane->length && FX_Mul(dotCA, dotA) <= dotC - curPlane->length)
			{
				sub_1FFF434(dotCA, dotA, dotC - curPlane->length, &localPos, &normalC, &normalA, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
			else if (dotB > dotC - curPlane->length && FX_Mul(dotBA, dotA) <= dotB)
			{
				sub_1FFF434(dotBA, dotB, dotA, &localPos, &normalA, &normalB, &squaredist);
				squaredist = ((s64)radius) * ((s64)radius) - squaredist;
				if (squaredist <= 0)
					continue;
			}
			else
			{
				squaredist = ((s64)radius) * ((s64)radius) - ((s64)dotA) * ((s64)dotA);
			}
		}
		if ((dotA <= dotB && dotB <= dotC - curPlane->length > 0 && dotC - curPlane->length > 0) ||
			(dotA <= dotB && dotB > 0) ||
			(dotA > dotB && dotA <= dotC - curPlane->length > 0 && dotC - curPlane->length > 0) ||
			(dotA > dotB && dotA > 0))
		{
			CP_SetSqrt64(squaredist);
			CP_WaitSqrt();
			length = CP_GetSqrtResultImm32() - dotN;

			if (length < 0)
				continue;
		}
		else
		{
			if (dotN < 0)
			{
				if (prevPos)
				{
					VecFx32 localPrevPos;
					VEC_Subtract(prevPos, &vertices[curPlane->vertexIdx], &localPrevPos);
					if (VEC_DotProduct(&localPrevPos, &normal) < 0)
					{
						//if ( flags & 0x20 )
						continue;
						if (VEC_DotProduct(&localPrevPos, &normalA) > 0)
							continue;
						if (VEC_DotProduct(&localPrevPos, &normalB) > 0)
							continue;
						if (VEC_DotProduct(&localPrevPos, &normalC) > curPlane->length)
							continue;
					}
				}
			}
		}

		VecFx32 thisdiff;
		thisdiff.x = (fx32)((((s64)length) * (s64)normal.x) >> 2);
		thisdiff.y = (fx32)((((s64)length) * (s64)normal.y) >> 2);
		thisdiff.z = (fx32)((((s64)length) * (s64)normal.z) >> 2);

		{
			if (thisdiff.x > diffMax.x)
				diffMax.x = thisdiff.x;
			if (thisdiff.x < diffMin.x)
				diffMin.x = thisdiff.x;

			if (thisdiff.y > diffMax.y)
				diffMax.y = thisdiff.y;
			if (thisdiff.y < diffMin.y)
				diffMin.y = thisdiff.y;

			if (thisdiff.z > diffMax.z)
				diffMax.z = thisdiff.z;
			if (thisdiff.z < diffMin.z)
				diffMin.z = thisdiff.z;

			if (length >= maxFloorLen)
			{
				maxFloorLen = length;
				floorDir = normal;
			}
		}

		// Store results.
		// TODO remove gravity from the equation
		VecFx32 gravity = { 0, -FX32_ONE, 0 };
		fx32 angle = FX_IDX_TO_DEG(FX_AcosIdx(VEC_DotProduct(&normal, &gravity)));
		if (angle > FX32_CONST(120)) // fixed variables for now
		{
			response->floor = 1;
		}
		else if (angle > FX32_CONST(85) && angle < FX32_CONST(95))
		{
			response->wall = 1;
		}
		else if (angle > FX32_CONST(180))
		{
			response->ceiling = 1;
		}

		response->type = curPlane->collisionType >> 8;

		// Let's assume we got something.
		succeeded = 1;
	}

	if (succeeded)
	{
		response->normal = floorDir;

		VecFx32 finalMax = diffMax;
		if (finalMax.x < diffWallMax.x)
			finalMax.x = diffWallMax.x;
		if (finalMax.y < diffWallMax.y)
			finalMax.y = diffWallMax.y;
		if (finalMax.z < diffWallMax.z)
			finalMax.z = diffWallMax.z;
		VecFx32 finalMin = diffMin;
		if (finalMin.x > diffWallMin.x)
			finalMin.x = diffWallMin.x;
		if (finalMin.y > diffWallMin.y)
			finalMin.y = diffWallMin.y;
		if (finalMin.z > diffWallMin.z)
			finalMin.z = diffWallMin.z;
		curPos->x += (finalMax.x + finalMin.x) >> 10;
		curPos->y += (finalMax.y + finalMin.y) >> 10;
		curPos->z += (finalMax.z + finalMin.z) >> 10;
		return 1;
	}

	return 0;
}

#include <nitro/itcm_end.h>
#pragma once

struct kcl_header
{
	uint32_t vertexOffset;
	uint32_t normalsOffset;
	uint32_t planesOffset;//-0x10
	uint32_t octreeOffset;
	fx32 unk1;
	VecFx32 octreeOrigin;
	uint32_t xMask;
	uint32_t yMask;
	uint32_t zMask;
	uint32_t coordShift;
	uint32_t yShift;
	uint32_t zShift;
	fx32 unk2;
};

struct kcl_plane
{
	fx32 length;
	uint16_t vertexIdx;
	uint16_t normalIdx;
	uint16_t normalAIdx;
	uint16_t normalBIdx;
	uint16_t normalCIdx;
	uint16_t collisionType;
};

struct kcl_response
{
	// The normal of the last checked plane.
	VecFx32 normal;
	// The water normal of the last checked plane which is water.
	VecFx32 waterNormal;
	// How far you are from the surface.
	uint32_t waterLength;
	// The collision type.
	uint32_t type;
	// Booleans to see what face you have touched.
	uint32_t floor : 1;
	uint32_t wall : 1;
	uint32_t ceiling : 1;
	uint32_t edge : 1;
	uint32_t water : 1;
};

int kcl_detect_collision(kcl_header* pKcl, VecFx32* curPos, VecFx32* prevPos, fx32 radius, kcl_response* response);
#pragma once

#include "../math/kcl.h"

class Collider 
{
protected:
	MtxFx43 mPreviousTransform;
	MtxFx43 mCurrentTransform;

	VecFx32 mPreviousPosition;
	VecFx32 mCurrentPosition;

public:
	enum Type
	{
		NONE,
		MESH,
		SPHERE
	};

	kcl_response mResponse;

	void Transform(const MtxFx43& transformation);
	void NoTransform();
	virtual bool Collide(Collider* collider) { return false; }
	virtual Type GetType() { return NONE; }

	void GetPosition(VecFx32* previousPosition, VecFx32* currentPosition);
	void SetPosition(VecFx32* previousPosition, VecFx32* currentPosition);
};
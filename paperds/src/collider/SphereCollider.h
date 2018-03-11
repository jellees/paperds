#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
private:
	fx32 mRadius;

public:
	SphereCollider(fx32 radius);

	bool Collide(Collider* collider);

	Type GetType()
	{
		return SPHERE;
	}

	fx32 GetRadius()
	{
		return mRadius;
	}
};
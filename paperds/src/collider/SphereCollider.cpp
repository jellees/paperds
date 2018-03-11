#include "common.h"
#include "SphereCollider.h"

SphereCollider::SphereCollider(fx32 radius)
{
	mRadius = radius;
}


bool SphereCollider::Collide(Collider* collider)
{
	return false;
}
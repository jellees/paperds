#include "common.h"
#include "math/kcl.h"
#include "SphereCollider.h"
#include "MeshCollider.h"


MeshCollider::MeshCollider(kcl_header* kcl)
{
	_collision = kcl;

	VecFx32 position = { 0,0,0 };
	this->SetPosition(&position, &position);
}

MeshCollider::~MeshCollider()
{
	if (_collision)
		delete _collision;
}


bool MeshCollider::Collide(Collider* collider)
{
	if (_collision)
	{
		switch (collider->GetType())
		{
		default:
		case MESH:
			return false;
		case SPHERE:
			return Sphere((SphereCollider*)collider);
		}
	}

	return false;
}


bool MeshCollider::Sphere(SphereCollider *sphere)
{
	VecFx32 previousPosition, currentPosition;
	sphere->GetPosition(&previousPosition, &currentPosition);

	VEC_Subtract(&currentPosition, &mCurrentPosition, &currentPosition);
	VEC_Subtract(&previousPosition, &mPreviousPosition, &previousPosition);

	// Store old values in case there is no collision.
	VecFx32 pp = previousPosition, cp = currentPosition;

	// Transforms mario's position relative.
	MTX_MultVec43(&currentPosition, &mPreviousTransform, &currentPosition);
	MTX_MultVec43(&previousPosition, &mPreviousTransform, &previousPosition);
	MTX_MultVec43(&currentPosition, &mCurrentTransform, &currentPosition);
	MTX_MultVec43(&previousPosition, &mCurrentTransform, &previousPosition);

	// Transforms mario's position absolute.
	MtxFx43 inverse;
	MTX_Inverse43(&mCurrentTransform, &inverse);
	MTX_MultVec43(&currentPosition, &inverse, &currentPosition);
	MTX_MultVec43(&previousPosition, &inverse, &previousPosition);

	int result = kcl_detect_collision(_collision, &currentPosition, &previousPosition, sphere->GetRadius(), &sphere->mResponse);

	MTX_MultVec43(&currentPosition, &mCurrentTransform, &currentPosition);
	MTX_MultVec43(&previousPosition, &mCurrentTransform, &previousPosition);

	if (!result)
	{
		previousPosition = pp;
		currentPosition = cp;
	}

	VEC_Add(&currentPosition, &mCurrentPosition, &currentPosition);
	VEC_Add(&previousPosition, &mPreviousPosition, &previousPosition);

	sphere->SetPosition(&previousPosition, &currentPosition);

	return result;
}
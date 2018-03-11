#include "common.h"
#include "Collider.h"


void Collider::Transform(const MtxFx43& transformation)
{
	MTX_Inverse43(&mCurrentTransform, &mPreviousTransform);
	mCurrentTransform = transformation;
}


void Collider::NoTransform()
{
	MTX_Inverse43(&mCurrentTransform, &mPreviousTransform);
	MtxFx43 transform;
	MTX_RotY43(&transform, FX_SinIdx(FX_DEG_TO_IDX(0)), FX_CosIdx(FX_DEG_TO_IDX(0)));
	mCurrentTransform = transform;
	this->Transform(mCurrentTransform);
}


void Collider::GetPosition(VecFx32* previousPosition, VecFx32* currentPosition)
{
	*previousPosition = mPreviousPosition;
	*currentPosition = mCurrentPosition;
}


void Collider::SetPosition(VecFx32* previousPosition, VecFx32* currentPosition)
{
	mPreviousPosition = *previousPosition;
	mCurrentPosition = *currentPosition;
}
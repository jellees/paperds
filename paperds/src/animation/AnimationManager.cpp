#include "common.h"
#include "AnimationManager.h"


/*AnimationManager::AnimationManager(NNSG3dResFileHeader* animationResource, const NNSG3dResMdl* modelPointer)
{
	_animationResources = new NNSG3dResFileHeader*[1];
	_animationResources[0] = animationResource;
	//extract the number of animations from the file itself
	_animationCount = ((NNSG3dResJntAnmSet*)NNS_G3dGetDataBlockHeaderByIdx(_animationResources[0], 0))->dict.numEntry;
	void* animationPointer;
	NNSFndAllocator allocator;
	NNS_FndInitAllocatorForExpHeap(&allocator, gHeapHandle, 4);
	_animations = new NNSG3dAnmObj*[_animationCount];
	for (int i = 0; i < _animationCount; i++)
	{
		animationPointer = NNS_G3dGetAnmByIdx(_animationResources[0], i);
		_animations[i] = NNS_G3dAllocAnmObj(&allocator, animationPointer, modelPointer);
		NNS_G3dAnmObjInit(_animations[i], animationPointer, modelPointer, NULL);
	}
	_currentAnimation = -1;
	_previousAnimation = -1;
}*/

AnimationManager::AnimationManager(const int animationCount, NNSG3dResFileHeader** animationResources, const NNSG3dResMdl* modelPointer)
{
	_animationCount = animationCount;
	_animationResources = animationResources;

	NNSFndAllocator allocator;
	NNS_FndInitAllocatorForExpHeap(&allocator, gHeapHandle, 4);

	_animations = new NNSG3dAnmObj*[_animationCount];

	for (int i = 0; i < _animationCount; i++)
	{
		void* animationPointer = NNS_G3dGetAnmByIdx(_animationResources[i], 0);
		_animations[i] = NNS_G3dAllocAnmObj(&allocator, animationPointer, modelPointer);
		NNS_G3dAnmObjInit(_animations[i], animationPointer, modelPointer, NULL);
	}

	_currentAnimation = -1;
	_previousAnimation = -1;
}

AnimationManager::~AnimationManager()
{
	for (int i = 0; i < _animationCount; i++)
		delete _animationResources[i];
	delete[] _animationResources;

	for (int i = 0; i < _animationCount; i++)
		delete _animations[i];
	delete[] _animations;
}


void AnimationManager::SetRender(NNSG3dRenderObj *render)
{
	_render = render;
}


void AnimationManager::SetAnimation(int animation, fx32 speed, bool loop)
{
	if (animation != _currentAnimation && animation < _animationCount)
	{
		_previousAnimation = _currentAnimation;
		_currentAnimation = animation;

		if (_previousAnimation != -1 && _animations[_previousAnimation])
			NNS_G3dRenderObjRemoveAnmObj(_render, _animations[_previousAnimation]);

		if(_animations[_currentAnimation])
			NNS_G3dRenderObjAddAnmObj(_render, _animations[_currentAnimation]);

		_animations[_currentAnimation]->frame = 0;

		_speed = speed;
		_loop = loop;

		_nextAnimation = -1;
	}
}


void AnimationManager::SetNextAnimation(int animation, fx32 speed, bool loop)
{
	if (animation < _animationCount)
	{
		_nextAnimation = animation;
		_nextSpeed = speed;
		_nextLoop = loop;
	}
}


void AnimationManager::Update()
{
	if (_currentAnimation != -1)
	{
		_animations[_currentAnimation]->frame += _speed;

		if (_animations[_currentAnimation]->frame >= NNS_G3dAnmObjGetNumFrame(_animations[_currentAnimation]))
		{
			if (_loop)
				_animations[_currentAnimation]->frame -= NNS_G3dAnmObjGetNumFrame(_animations[_currentAnimation]);
			else if (_nextAnimation != -1)
				this->SetAnimation(_nextAnimation, _nextSpeed, _nextLoop);
		}
	}
}


void AnimationManager::IncreaseFrame(fx32 speed)
{
	if (_currentAnimation != -1)
	{
		_animations[_currentAnimation]->frame += speed;
	}
}
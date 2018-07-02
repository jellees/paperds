#pragma once

/***************************************/
/* Animation Manager written by Jesse  */
/* Modified by Ermelber                */
/***************************************/

#define ANIMATION_DEFAULT_SPEED FX32_CONST(2)

class AnimationManager
{
private:
	NNSG3dResFileHeader** _animationResources;
	NNSG3dAnmObj** _animations;
	NNSG3dRenderObj* _render;

	int _animationCount;

	int _previousAnimation;
	
	int _currentAnimation;
	int _nextAnimation;

	bool _loop;
	bool _nextLoop;

	fx32 _speed;
	fx32 _nextSpeed;

public:
	//AnimationManager(NNSG3dResFileHeader* animationResource, const NNSG3dResMdl* modelPointer);
	AnimationManager(const int animationCount, NNSG3dResFileHeader** animationResources, const NNSG3dResMdl* modelPointer);
	~AnimationManager();

	void SetRender(NNSG3dRenderObj *render);
	void SetAnimation(int animation, fx32 speed, bool loop = true);
	void SetNextAnimation(int animation, fx32 speed, bool loop = true);
	void Update();
	void IncreaseFrame(fx32 speed);

	int GetCurrentAnimation()
	{
		return _currentAnimation;
	}

	int GetFrameCount()
	{
		if (_currentAnimation == -1) 
			return 0;
		return NNS_G3dAnmObjGetNumFrame(_animations[_currentAnimation]);
	}
};
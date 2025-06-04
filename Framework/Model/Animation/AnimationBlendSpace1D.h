#pragma once

class AnimationClip;

class AnimationBlendSpace1D
{
public:
	explicit AnimationBlendSpace1D(const CSkeletal * InSkeleton);
	~AnimationBlendSpace1D();
	void SetHorizontalRange(float Value1, float Value2);
	void AddAnimation(AnimationClip * Anim, float At);
	void Play(float Value, float Time);
private:
	float Horizontal;
	float Min;
	float Max;
	bool bWrap = false;
	map<float, AnimationClip *> Animations;
};

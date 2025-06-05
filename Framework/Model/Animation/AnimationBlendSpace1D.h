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
	float GetDuration() const;
	float GetBlendSpaceLength() const;

	void GetTargetAnimations(float Value, const AnimationClip ** OutAnim1, const AnimationClip ** OutAnim2) const;

private:
	float WrapValue(float InValue) const;
	void RefreshDuration();
	const CSkeletal * Skeleton;
	float Duration = -1.f;
	float PlayRate = 1.f;
	float Min = 0.f;
	float Max = 10.f;
	bool bWrap = false;
	map<float, AnimationClip *> Animations;
};

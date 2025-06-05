#pragma once
#include "AnimationBlendSpace1D.h"

class AnimationClip;

class AnimationController
{
private:
	struct AnimationInfoDesc
	{
		int	  CurrentFrame = -1;
		int   NextFrame  = -1;
		float CurrentTime = 0.0f;
		float LerpRate = 0.0f;
	};
public:
	explicit AnimationController(CSkeletal * InSkeletal);
	~AnimationController();
	void PlaySingleAnimationClip(const AnimationClip* Clip, float DeltaSecond);
	void PlayAnimationBlendSpace1D(const AnimationBlendSpace1D* BlendSpace1D, float DeltaSecond, float Value);
	void UpdateAnimationFrameData(float DeltaSecond);
	void Tick();

	void SetCurrentAnimation(const AnimationClip * Clip);
	void SetCurrentBlendSpace(const AnimationBlendSpace1D * BlendSpace1D);

	void CalculateBoneMatrices() const;

private:
	AnimationInfoDesc AnimationData;
	ConstantBuffer * CB_AnimationInfo;
	HlslComputeShader * AnimationKeyFrameCalculator = nullptr;
	// HlslComputeShader * AnimationKeyFrameBlender = nullptr;
	map<string, AnimationClip *> AnimationClips;

	CSkeletal * TargetSkeletal = nullptr;
	const AnimationClip * CurrentAnimation = nullptr;
	const AnimationBlendSpace1D * CurrentBlendSpace = nullptr;
};

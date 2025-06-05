#pragma once
#include "AnimationBlendSpace1D.h"

class AnimationClip;

class AnimationController
{
private:
	struct AnimationInfoDesc
	{
		int	  KeyFrameCurr = -1;
		int   KeyFrameNext  = -1;
		float LerpRate = 0.0f;
		float CurrentTime = 0.0f;
	};
	struct BlendSpace1DInfoDesc
	{
		int KeyFrameCurr[2] = {-1, -1};
		int KeyFrameNext[2] = {-1, -1};
		
		float LerpRate[2] = {0.0f, 0.0f};
		float Alpha = 0.5f;
		float CurrentTime = 0.0f;
	};
public:
	explicit AnimationController(CSkeletal * InSkeletal);
	~AnimationController();
	void PlaySingleAnimationClip(const AnimationClip* Clip, float DeltaSecond);
	void PlayAnimationBlendSpace1D(const AnimationBlendSpace1D* BlendSpace1D, float DeltaSecond, float Value);
	void UpdateAnimationFrameData(float DeltaSecond);
	void Tick();

	void SetCurrentAnimation(AnimationClip * Clip);
	void SetCurrentBlendSpace(AnimationBlendSpace1D * BlendSpace1D);

private:
	AnimationInfoDesc AnimationData;
	BlendSpace1DInfoDesc BlendSpace1DData;
	ConstantBuffer * CB_AnimationInfo;
	ConstantBuffer * CB_BlendSpace1DInfo;
	HlslComputeShader * AnimationClipPlayer = nullptr;
	HlslComputeShader * AnimationBlendSpace1DPlayer = nullptr;
	// HlslComputeShader * AnimationKeyFrameBlender = nullptr;
	map<string, AnimationClip *> AnimationClips;

	CSkeletal * TargetSkeletal = nullptr;
	AnimationClip * CurrentAnimation = nullptr;
	AnimationBlendSpace1D * CurrentBlendSpace = nullptr;
};

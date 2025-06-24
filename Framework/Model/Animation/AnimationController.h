#pragma once
#include "AnimationBlendSpace1D.h"
#include "AnimationClip.h"

class AnimationBlendSpace2D;
class AnimationClip;

class AnimationController
{
private:
	struct AnimationInfoDesc
	{
		int	  KeyFrameCurr = -1;
		int   KeyFrameNext  = -1;
		float LerpRate = 0.0f;
		float Weight = 1.0f;
	};
	struct Animation_ConstantDesc
	{
		Animation_ConstantDesc() = default;
		Animation_ConstantDesc(const array<AnimationInfoDesc, 3> & Anims, const array<float, 3> & Weights)
		: AnimData{ Anims[0], Anims[1], Anims[2] }
		{
			AnimData[0].Weight = Weights[0];
			AnimData[1].Weight = Weights[1];
			AnimData[2].Weight = Weights[2];
		}
		AnimationInfoDesc AnimData[3];
	};
public:
	explicit AnimationController(CSkeletal * InSkeletal);
	~AnimationController();
	void PlaySingleAnimationClip(const AnimationClip* InClip, float DeltaSecond);
	void PlayAnimationBlendSpace1D(const AnimationBlendSpace1D* InBlendSpace1D, float DeltaSecond, float Value);
	void PlayAnimationBlendSpace2D(const AnimationBlendSpace2D* InBlendSpace2D, float DeltaSecond, float ValueHorizontal, float ValueVertical);
	void UpdateAnimationFrameData(float DeltaSecond);
	void Tick();

	void SetCurrentAnimation(AnimationClip * Clip);
	void SetCurrentBlendSpace(AnimationBlendSpace1D * BlendSpace1D);
	void SetCurrentBlendSpace(AnimationBlendSpace2D * BlendSpace2D);

private:
	static AnimationInfoDesc GetInfo(const AnimationClip * Clip, float InNormalizedPlayTime);

	float NormalizedPlayTime = 0.0f;
	
	Animation_ConstantDesc Animation_ConstantData;
	// TODO : unique_ptr
	ConstantBuffer * CB_AnimationData;
	// TODO : unique_ptr
	ComputeShader * KeyFrameAnimationCalculator = nullptr;
	map<string, AnimationClip *> AnimationClips;

	// TODO : shared_ptr
	CSkeletal * TargetSkeletal = nullptr;
	// TODO : shared_ptr
	AnimationClip * CurrentAnimation = nullptr;
	// TODO : shared_ptr
	AnimationBlendSpace1D * CurrentBlendSpace = nullptr;
	// TODO : shared_ptr
	AnimationBlendSpace2D * CurrentBlendSpace2D = nullptr;
};

#pragma once
#include "AnimationBlendSpace1D.h"

class AnimationBlendSpace2D;
class AnimationClip;

class AnimationController
{
private:
	float CurrentFrame = 0.0f;
	struct AnimationInfoDesc
	{
		int	  KeyFrameCurr = -1;
		int   KeyFrameNext  = -1;
		float LerpRate = 0.0f;
		float Weight = 1.0f;
	};
	struct BlendSpace1DInfoDesc
	{
		BlendSpace1DInfoDesc() = default;
		BlendSpace1DInfoDesc(const AnimationInfoDesc& A, const AnimationInfoDesc& B, float Alpha)
			: AnimData{ A, B }
		{
			AnimData[0].Weight = Alpha;
			AnimData[1].Weight = 1.f - Alpha;
		}
		AnimationInfoDesc AnimData[2];
	};
	struct BlendSpace2DInfoDesc
	{
		BlendSpace2DInfoDesc() = default;
		BlendSpace2DInfoDesc(const array<AnimationInfoDesc, 3> & Anims, const array<float, 3> & Weights)
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
	void SetCurrentBlendSpace(AnimationBlendSpace2D* BlendSpace1D);

private:
	static AnimationInfoDesc GetInfo(const AnimationClip * Clip, float InCurrentFrame);
	
	AnimationInfoDesc AnimationData;
	BlendSpace1DInfoDesc BlendSpace1DData;
	BlendSpace2DInfoDesc BlendSpace2DData;
	
	ConstantBuffer * CB_AnimationInfo;
	ConstantBuffer * CB_BlendSpace1DInfo;
	ConstantBuffer * CB_BlendSpace2DInfo;
	
	HlslComputeShader * AnimationClipPlayer = nullptr;
	HlslComputeShader * AnimationBlendSpace1DPlayer = nullptr;
	HlslComputeShader * AnimationBlendSpace2DPlayer = nullptr;
	map<string, AnimationClip *> AnimationClips;

	CSkeletal * TargetSkeletal = nullptr;
	AnimationClip * CurrentAnimation = nullptr;
	AnimationBlendSpace1D * CurrentBlendSpace = nullptr;
	AnimationBlendSpace2D * CurrentBlendSpace2D = nullptr;
};

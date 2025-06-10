#pragma once
#include "AnimationBlendSpace1D.h"

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
		float CurrentFrame = 0.0f;
	};
	struct BlendSpace1DInfoDesc
	{
		BlendSpace1DInfoDesc() = default;
		BlendSpace1DInfoDesc(const AnimationInfoDesc& A, const AnimationInfoDesc& B, float Alpha)
			: KeyFrameCurr{A.KeyFrameCurr, B.KeyFrameCurr}
			, KeyFrameNext{A.KeyFrameNext, B.KeyFrameNext}
			, LerpRate{A.LerpRate, B.LerpRate}
			, Alpha(Alpha)
			, CurrentFrame(A.CurrentFrame) {}
		int KeyFrameCurr[2] = {-1, -1};
		int KeyFrameNext[2] = {-1, -1};
		float LerpRate[2] = {0.0f, 0.0f};
		float Alpha = 0.5f;
		float CurrentFrame = 0.0f;
	};
	struct BlendSpace2DInfoDesc
	{
		BlendSpace2DInfoDesc() = default;
		BlendSpace2DInfoDesc(const array<AnimationInfoDesc, 3> & Anims, const array<float, 3> & Weights)
			: KeyFrameCurr{Anims[0].KeyFrameCurr, Anims[1].KeyFrameCurr, Anims[2].KeyFrameCurr}
		  , KeyFrameNext{Anims[0].KeyFrameNext, Anims[1].KeyFrameNext, Anims[2].KeyFrameNext}
		  , LerpRate{Anims[0].LerpRate, Anims[1].LerpRate, Anims[2].LerpRate}
		  , Weight{Weights[0], Weights[1], Weights[2]}
		{ }
		int KeyFrameCurr[3] = {-1,};
		int KeyFrameNext[3] = {-1,};
		float LerpRate[3] = {0.0f,};
		float Weight[3] = {0.5f,};
		float CurrentFrame = 0.0f;
		float Padding[3] = {0.f, 0.f, 0.f};
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

private:
	AnimationInfoDesc GetInfo(const AnimationClip * Clip, float InCurrentFrame);
	
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
};

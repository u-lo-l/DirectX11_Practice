#pragma once

class AnimationClip;

class AnimationController
{
private:
	struct FrameDesc
	{
		float	CurrentTime = -1;
		int		CurrentFrame = -1;
		int		NextFrame = -1;
		float	Padding;
	};
	struct AnimationBlendingDesc
	{
		float BlendingDuration = 0.1f;
		float ElapsedBlendTime = 0.0f;
		float Padding[2];

		FrameDesc Current;
		FrameDesc Next;
	};
public:
	explicit AnimationController(CSkeletal * InSkeletal);
	~AnimationController();
	void Tick();

	void SetCurrentAnimation(const AnimationClip * Clip);
	void SetNextAnimation(const AnimationClip * Clip);

	void CalculateBoneMatrices(
		float Time,
		array<Matrix, CSkeletal::MAX_BONE_COUNT> & OutBoneMatrix
	);
	void CalculateBoneMatrices(
		array<Matrix, CSkeletal::MAX_BONE_COUNT> & OutBoneMatrix
	);

private:
	AnimationBlendingDesc AnimationInfo;
	
	HlslComputeShader * AnimationKeyFrameCalculator = nullptr;
	HlslComputeShader * AnimationKeyFrameBlender = nullptr;
	map<string, AnimationClip *> AnimationClips;
	
	CSkeletal * TargetSkeletal = nullptr;
	const AnimationClip * CurrentAnimation = nullptr;
	const AnimationClip * NextAnimation = nullptr;
};

#pragma once

class AnimationClip;

class AnimationController
{
public:
	AnimationController();
	~AnimationController();
	
	static void CalculateBoneMatrices(
		const AnimationClip * AnimationCurr,
		float Time,
		array<Matrix, CSkeletal::MAX_BONE_COUNT> & OutBoneMatrix
	);
	static void CalculateBoneMatrices(
		const AnimationClip * AnimationCurr,
		const AnimationClip * AnimationNext,
		array<Matrix, CSkeletal::MAX_BONE_COUNT> & OutBoneMatrix
	);

private:
	HlslComputeShader * AnimationKeyFrameCalculator = nullptr;
	map<string, AnimationClip *> AnimationClips;
};

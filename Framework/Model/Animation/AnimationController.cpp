#include "framework.h"
#include "AnimationController.h"

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::CalculateBoneMatrices
(
	const AnimationClip* AnimationCurr,
	float Time,
	array<Matrix, CSkeletal::MAX_BONE_COUNT>& OutBoneMatrix
)
{
}

void AnimationController::CalculateBoneMatrices
(
	const AnimationClip* AnimationCurr,
	const AnimationClip* AnimationNext,
	array<Matrix, CSkeletal::MAX_BONE_COUNT>& OutBoneMatrix
)
{
}

#include "framework.h"
#include "AnimationController.h"

#include "AnimationClip.h"

AnimationController::AnimationController(CSkeletal* InSkeletal)
	: AnimationInfo(), TargetSkeletal(InSkeletal)
{
	ASSERT(!!TargetSkeletal, "Skeleton Not Valid");
	// AnimationKeyFrameCalculator = new HlslComputeShader(
	// 	L"Mesh/Animation/KeyFrameCalculator.hlsl",
	// 	nullptr
	// );
	// AnimationKeyFrameCalculator->SetDispatchSize(1,1,1);
	// AnimationKeyFrameBlender = new HlslComputeShader(
	// 	L"Mesh/Animation/KeyFrameBlender.hlsl",
	// 	nullptr
	// );
	// AnimationKeyFrameBlender->SetDispatchSize(1,1,1);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(AnimationKeyFrameCalculator);
	SAFE_DELETE(AnimationKeyFrameBlender);
}

void AnimationController::Tick()
{
	const float DeltaSecond = sdt::SystemTimer::Get()->GetDeltaTime();
	array<Matrix, CSkeletal::MAX_BONE_COUNT> & BoneMatrix = TargetSkeletal->GetBoneMatrices();
	if (!!CurrentAnimation)
	{
		const float CurrentTime = AnimationInfo.Current.CurrentTime;
		const float NextTime = CurrentAnimation->CalculateNextAnimTime(CurrentTime, DeltaSecond);
		CalculateBoneMatrices(0.f, BoneMatrix);
		if (NextTime < 0)
		{
			CurrentAnimation = nullptr;
			AnimationInfo.Current.CurrentTime = -1;
			AnimationInfo.Current.CurrentFrame = -1;
			AnimationInfo.Current.NextFrame = -1;
		}
	}
}

void AnimationController::SetCurrentAnimation(const AnimationClip* const Clip)
{
	CurrentAnimation = Clip;

	AnimationInfo.Current.CurrentFrame = 0;
	AnimationInfo.Current.CurrentTime = 0;
	AnimationInfo.Current.NextFrame = 0;

	AnimationInfo.Next.CurrentTime = -1;
	AnimationInfo.Next.CurrentFrame = -1;
	AnimationInfo.Next.NextFrame = -1;
}

void AnimationController::SetNextAnimation(const AnimationClip* Clip)
{
	NextAnimation = Clip;
	AnimationInfo.BlendingDuration = 0.1f;
	AnimationInfo.ElapsedBlendTime = 0.0f;

	AnimationInfo.Next.CurrentTime = 0;
	AnimationInfo.Next.CurrentFrame = 0;
	AnimationInfo.Next.NextFrame = 0;
}

void AnimationController::CalculateBoneMatrices
(
	float Time,
	array<Matrix, CSkeletal::MAX_BONE_COUNT>& OutBoneMatrix
)
{
	ASSERT(!!TargetSkeletal, "Skeletal Invalid")
	const Texture * const KeyFrameTexture = CurrentAnimation->GetKeyFrameTexture();
	auto & BoneMatrices = TargetSkeletal->GetBoneMatrices();

	KeyFrameTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader));
}

void AnimationController::CalculateBoneMatrices
(
	array<Matrix, CSkeletal::MAX_BONE_COUNT>& OutBoneMatrix
)
{
}

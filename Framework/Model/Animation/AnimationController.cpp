#include "framework.h"
#include "AnimationController.h"
#include "AnimationController.h"

#include "AnimationBlendSpace1D.h"
#include "AnimationClip.h"

AnimationController::AnimationController(CSkeletal* InSkeletal)
	: TargetSkeletal(InSkeletal)
{
	ASSERT(!!TargetSkeletal, "Skeleton Not Valid");

	CB_AnimationInfo = new ConstantBuffer(
		static_cast<UINT>(ShaderType::ComputeShader),
		0,
		nullptr,
		"",
		sizeof(AnimationInfoDesc),
		false
	);

	const vector<D3D_SHADER_MACRO> Defines = {
		{"THREAD_X", "16"},
		{nullptr, nullptr}
	};
	AnimationKeyFrameCalculator = new HlslComputeShader(
		L"Mesh/Animation/KeyFrameCalculator.hlsl",
		Defines.data(),
		"CSMain",
		true
	);
	AnimationKeyFrameCalculator->SetDispatchSize(16, 1, 1);
	// AnimationKeyFrameBlender = new HlslComputeShader(
	// 	L"Mesh/Animation/KeyFrameBlender.hlsl",
	// 	nullptr
	// );
	// AnimationKeyFrameBlender->SetDispatchSize(1,1,1);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(AnimationKeyFrameCalculator);
	// SAFE_DELETE(AnimationKeyFrameBlender);
	SAFE_DELETE(CB_AnimationInfo);
}

void AnimationController::PlaySingleAnimation
(
	const AnimationClip * Clip,
	const float DeltaSecond
)
{
	if (!Clip)
		return;
	
	const float CurrentTime = AnimationData.CurrentTime;
	const float CurrentFrameTime = Clip->GetCurrentFrameTime(CurrentTime);
	const float NextFrameTime = Clip->GetNextFrameTime(CurrentTime);
	float LerpRate = 0;
	
	if (NextFrameTime > 0 && CurrentTime > NextFrameTime)
		LerpRate = (CurrentTime - CurrentFrameTime) / (NextFrameTime - CurrentFrameTime);
	AnimationData = {
		Clip->GetCurrentFrame(CurrentTime),
		Clip->GetNextFrame(CurrentTime),
		CurrentTime,
		LerpRate
	};
	CB_AnimationInfo->UpdateData(&AnimationData, sizeof(AnimationInfoDesc));
	CalculateBoneMatrices();
	const float NextTime = Clip->CalculateNextAnimTime(CurrentTime, DeltaSecond);
	if (NextTime > 0)
		AnimationData.CurrentTime = NextTime;
}

void AnimationController::PlayAnimationBlendSpace1D
(
	const AnimationBlendSpace1D * BlendSpace1D,
	const float DeltaSecond,
	const float Value
)
{

}

void AnimationController::UpdateAnimationFrameData(float DeltaSecond)
{
	
}

void AnimationController::Tick()
{
	const float DeltaSecond = sdt::SystemTimer::Get()->GetDeltaTime();
	UpdateAnimationFrameData(DeltaSecond);
	PlaySingleAnimation(CurrentAnimation, DeltaSecond);
}

void AnimationController::SetCurrentAnimation(const AnimationClip* const Clip)
{
	CurrentAnimation = Clip;
}

void AnimationController::SetNextAnimation(const AnimationClip* Clip)
{
	NextAnimation = Clip;
}

void AnimationController::CalculateBoneMatrices() const
{
	ASSERT(!!TargetSkeletal, "Skeletal Invalid")
	const Texture * const KeyFrameTexture = CurrentAnimation->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	CB_AnimationInfo->BindToGPU();
	KeyFrameTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV
	AnimationKeyFrameCalculator->Dispatch();
	SB_BoneMatrices->UpdateSRV();
}

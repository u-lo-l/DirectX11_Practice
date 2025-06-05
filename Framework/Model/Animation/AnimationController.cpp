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

void AnimationController::PlaySingleAnimationClip
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
	if (!BlendSpace1D)
		return;
	
	const AnimationClip * Anim1;
	const AnimationClip * Anim2;
	BlendSpace1D->GetTargetAnimations(Value, &Anim1, &Anim2);
	if (Anim1 == nullptr && Anim2 == nullptr)
		return ;
	if (Anim1 == Anim2)
	{
		PlaySingleAnimationClip(Anim2, DeltaSecond);
		return;
	}
	const float CurrentTime = AnimationData.CurrentTime;
	const float CurrentFrameTime1 = Anim1->GetCurrentFrameTime(CurrentTime);
	const float NextFrameTime1 = Anim1->GetNextFrameTime(CurrentTime);
	float LerpRate1 = 0;
	if (NextFrameTime1 > 0 && CurrentTime > NextFrameTime1)
		LerpRate1 = (CurrentTime - CurrentFrameTime1) / (NextFrameTime1 - CurrentFrameTime1);

	const float CurrentFrameTime2 = Anim2->GetCurrentFrameTime(CurrentTime);
	const float NextFrameTime2 = Anim2->GetNextFrameTime(CurrentTime);
	float LerpRate2 = 0;
	if (NextFrameTime2 > 0 && CurrentTime > NextFrameTime2)
		LerpRate2 = (CurrentTime - CurrentFrameTime2) / (NextFrameTime2 - CurrentFrameTime2);

	
}

void AnimationController::UpdateAnimationFrameData(float DeltaSecond)
{
	
}

void AnimationController::Tick()
{
	const float DeltaSecond = sdt::SystemTimer::Get()->GetDeltaTime();
	UpdateAnimationFrameData(DeltaSecond);
	PlaySingleAnimationClip(CurrentAnimation, DeltaSecond);
}

void AnimationController::SetCurrentAnimation(const AnimationClip* const Clip)
{
	CurrentBlendSpace = nullptr;
	CurrentAnimation = Clip;
	AnimationData = {};
}

void AnimationController::SetCurrentBlendSpace(const AnimationBlendSpace1D* BlendSpace1D)
{
	CurrentAnimation = nullptr;
	CurrentBlendSpace = BlendSpace1D;
	AnimationData = {};
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

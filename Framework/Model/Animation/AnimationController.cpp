#include "framework.h"
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
	CB_BlendSpace1DInfo = new ConstantBuffer(
		static_cast<UINT>(ShaderType::ComputeShader),
		0,
		nullptr,
		"",
		sizeof(BlendSpace1DInfoDesc),
		false
	);

	const vector<D3D_SHADER_MACRO> Defines = {
		{"THREAD_X", "32"},
		{nullptr, nullptr}
	};
	AnimationClipPlayer = new HlslComputeShader(
		L"Mesh/Animation/AnimationClipPlayer.hlsl",
		Defines.data(),
		"CSMain",
		false
	);
	AnimationClipPlayer->SetDispatchSize(8, 1, 1);
	AnimationBlendSpace1DPlayer = new HlslComputeShader(
		L"Mesh/Animation/BlendSpace1DPlayer.hlsl",
		Defines.data(),
		"CSMain",
		true
	);
	AnimationBlendSpace1DPlayer->SetDispatchSize(8, 1, 1);
	// AnimationKeyFrameBlender = new HlslComputeShader(
	// 	L"Mesh/Animation/KeyFrameBlender.hlsl",
	// 	nullptr
	// );
	// AnimationKeyFrameBlender->SetDispatchSize(1,1,1);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(AnimationClipPlayer);
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
	
	const float CurrentFrame = Clip->GetCurrentFrame(AnimationData.CurrentTime);
	const int KeyFrameCurr = Clip->GetKeyFrameCurr(CurrentFrame);
	const int KeyFrameNext = Clip->GetKeyFrameNext(CurrentFrame);
	float LerpRate = 0;
	
	if (KeyFrameNext > 0 && CurrentFrame > (float)KeyFrameCurr)
		LerpRate = (CurrentFrame - (float)KeyFrameCurr) / (float)(KeyFrameNext - KeyFrameCurr);
	
	AnimationData = {
		KeyFrameCurr,
		KeyFrameNext,
		LerpRate,
		CurrentFrame
	};
	CB_AnimationInfo->UpdateData(&AnimationData, sizeof(AnimationInfoDesc));
	
	const Texture * const KeyFrameTexture = Clip->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	CB_AnimationInfo->BindToGPU();
	KeyFrameTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV
	
	AnimationClipPlayer->Dispatch();
	
	const float NextTime = Clip->GetNextFrame(CurrentFrame, DeltaSecond);
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
	float Alpha;
	BlendSpace1D->GetTargetAnimations(Value, &Anim1, &Anim2, &Alpha);
	if (Anim1 == nullptr && Anim2 == nullptr)
		return ;
	if (Anim1 == Anim2)
	{
		AnimationData.CurrentTime = BlendSpace1DData.CurrentTime;
		PlaySingleAnimationClip(Anim2, DeltaSecond);
		BlendSpace1DData.CurrentTime = AnimationData.CurrentTime;
		return;
	}
	
	const float CurrentTime = BlendSpace1DData.CurrentTime;

	const float CurrentFrame1 = Anim1->GetCurrentFrame(CurrentTime);
	const int KeyFrameCurr1 = Anim1->GetKeyFrameCurr(CurrentFrame1);
	const int KeyFrameNext1 = Anim1->GetKeyFrameNext(CurrentFrame1);
	float LerpRate1 = 0;
	if (KeyFrameNext1 > 0 && CurrentFrame1 > (float)KeyFrameCurr1)
		LerpRate1 = (CurrentFrame1 - (float)KeyFrameCurr1) / (float)(KeyFrameNext1 - KeyFrameCurr1);

	const float CurrentFrame2 = Anim2->GetCurrentFrame(CurrentTime);
	const int KeyFrameCurr2 = Anim2->GetKeyFrameCurr(CurrentFrame2);
	const int KeyFrameNext2 = Anim2->GetKeyFrameNext(CurrentFrame2);
	float LerpRate2 = 0;
	if (KeyFrameNext2 > 0 && CurrentFrame2 > (float)KeyFrameCurr2)
		LerpRate2 = (CurrentFrame2 - (float)KeyFrameCurr2) / (float)(KeyFrameNext2 - KeyFrameCurr2);

	BlendSpace1DData = {
		{KeyFrameCurr1, KeyFrameCurr2},
		{KeyFrameNext1, KeyFrameNext2},
		{LerpRate1, LerpRate2},
		Alpha,
		CurrentTime
	};
	CB_BlendSpace1DInfo->UpdateData(&BlendSpace1DData, sizeof(BlendSpace1DInfoDesc));

	const Texture * const AnimTexture1 = Anim1->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = Anim2->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();
	
	CB_BlendSpace1DInfo->BindToGPU();
	AnimTexture1->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture2->BindToGPU(1, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV

	AnimationBlendSpace1DPlayer->Dispatch();

	const float NextTime = BlendSpace1D->GetNextFrame(CurrentTime, DeltaSecond);
	if (NextTime > 0)
		BlendSpace1DData.CurrentTime = NextTime;
}

void AnimationController::UpdateAnimationFrameData(float DeltaSecond)
{
	
}

void AnimationController::Tick()
{
	const float DeltaSecond = sdt::SystemTimer::Get()->GetDeltaTime();
	UpdateAnimationFrameData(DeltaSecond);
	if (!!CurrentAnimation)
		PlaySingleAnimationClip(CurrentAnimation, DeltaSecond);
	if (!!CurrentBlendSpace)
	{
		static float WalkSpeed = 0;
		ImGui::SliderFloat("Walk Speed", &WalkSpeed, 0.0f, 1.0f);
		PlayAnimationBlendSpace1D(CurrentBlendSpace, DeltaSecond, WalkSpeed);
	}
}

void AnimationController::SetCurrentAnimation(AnimationClip * Clip)
{
	CurrentBlendSpace = nullptr;
	CurrentAnimation = Clip;
	AnimationData = {};
}

void AnimationController::SetCurrentBlendSpace(AnimationBlendSpace1D* BlendSpace1D)
{
	CurrentAnimation = nullptr;
	CurrentBlendSpace = BlendSpace1D;
	BlendSpace1DData = {};
}


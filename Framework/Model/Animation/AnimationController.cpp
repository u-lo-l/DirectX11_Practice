#include "framework.h"
#include "AnimationController.h"

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
	CB_BlendSpace2DInfo = new ConstantBuffer(
		static_cast<UINT>(ShaderType::ComputeShader),
		0,
		nullptr,
		"",
		sizeof(BlendSpace2DInfoDesc),
		false
	);

	vector<D3D_SHADER_MACRO> Defines = {
		{"THREAD_X", "32"},
		{"CLIP", ""},
		{nullptr, nullptr}
	};
	AnimationClipPlayer = new HlslComputeShader(
		L"Mesh/Animation/AnimationPlayer.hlsl",
		Defines.data(),
		"CSMain",
		false
	);
	AnimationClipPlayer->SetDispatchSize(8, 1, 1);

	Defines = {
		{"THREAD_X", "32"},
		{"BLEND1D", ""},
		{nullptr, nullptr}
	};
	AnimationBlendSpace1DPlayer = new HlslComputeShader(
		L"Mesh/Animation/AnimationPlayer.hlsl",
		Defines.data(),
		"CSMain",
		true
	);
	AnimationBlendSpace1DPlayer->SetDispatchSize(8, 1, 1);

	Defines = {
		{"THREAD_X", "32"},
		{"BLEND2D", ""},
		{nullptr, nullptr}
	};
	AnimationBlendSpace2DPlayer = new HlslComputeShader(
		L"Mesh/Animation/AnimationPlayer.hlsl",
		Defines.data(),
		"CSMain",
		true
	);
	AnimationBlendSpace2DPlayer->SetDispatchSize(8, 1, 1);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(AnimationClipPlayer);
	// SAFE_DELETE(AnimationKeyFrameBlender);
	SAFE_DELETE(CB_AnimationInfo);
}

void AnimationController::PlaySingleAnimationClip
(
	const AnimationClip * InClip,
	const float DeltaSecond
)
{
	if (!InClip)
		return;

	const AnimationInfoDesc CurrentClipPlayingInfo = GetInfo(InClip, this->CurrentFrame);
	AnimationData = CurrentClipPlayingInfo;
	CB_AnimationInfo->UpdateData(&AnimationData, sizeof(AnimationInfoDesc));
	
	const Texture * const KeyFrameTexture = InClip->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	CB_AnimationInfo->BindToGPU();
	KeyFrameTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV
	
	AnimationClipPlayer->Dispatch();
	
	const float NextTime = InClip->GetNextFrame(this->CurrentFrame, DeltaSecond);
	if (NextTime > 0)
		this->CurrentFrame = NextTime;
}

void AnimationController::PlayAnimationBlendSpace1D
(
	const AnimationBlendSpace1D * InBlendSpace1D,
	const float DeltaSecond,
	const float Value
)
{
	if (!InBlendSpace1D)
		return;
	
	const AnimationClip * Anim1;
	const AnimationClip * Anim2;
	float Alpha;
	InBlendSpace1D->GetTargetAnimations(Value, &Anim1, &Anim2, &Alpha);
	if (Anim1 == nullptr && Anim2 == nullptr)
		return ;
	if (Anim1 == Anim2)
	{
		PlaySingleAnimationClip(Anim2, DeltaSecond);
		return;
	}
	
		const AnimationInfoDesc Anim1PlayingInfo = GetInfo(Anim1, this->CurrentFrame);
	const AnimationInfoDesc Anim2PlayingInfo = GetInfo(Anim2, this->CurrentFrame);
	BlendSpace1DData = BlendSpace1DInfoDesc(Anim1PlayingInfo, Anim2PlayingInfo, Alpha);
	CB_BlendSpace1DInfo->UpdateData(&BlendSpace1DData, sizeof(BlendSpace1DInfoDesc));

	const Texture * const AnimTexture1 = Anim1->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = Anim2->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();
	
	CB_BlendSpace1DInfo->BindToGPU();
	AnimTexture1->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture2->BindToGPU(1, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV

	AnimationBlendSpace1DPlayer->Dispatch();

	const float NextTime = InBlendSpace1D->GetNextFrame(this->CurrentFrame, DeltaSecond);
	if (NextTime > 0)
		this->CurrentFrame = NextTime;
}

void AnimationController::PlayAnimationBlendSpace2D
(
	const AnimationBlendSpace2D * InBlendSpace2D,
	const float DeltaSecond,
	const float ValueHorizontal,
	const float ValueVertical
)
{
	if (AnimationBlendSpace2DPlayer == nullptr)
		return ;
	if (!InBlendSpace2D)
		return;
	
	array<const AnimationClip *, 3> Clips;
	array<float, 3> Weights;
	InBlendSpace2D->GetTargetAnimations({ValueHorizontal, ValueVertical},Clips,Weights);
	ASSERT((Weights[0] > -Math::EPSILON && Weights[1] > -Math::EPSILON && Weights[2] > -Math::EPSILON), "Weight Not Valid : NegativeValue");
	ASSERT(false == (Math::IsZero(Weights[0]) && Math::IsZero(Weights[1]) && Math::IsZero(Weights[2])), "Weight Not Valid : All Zero");
	
	const array<AnimationInfoDesc, 3> Anim1PlayingInfos {
		GetInfo(Clips[0], this->CurrentFrame),
		GetInfo(Clips[1], this->CurrentFrame),
		GetInfo(Clips[2], this->CurrentFrame)
	};
	BlendSpace2DData = BlendSpace2DInfoDesc(Anim1PlayingInfos, Weights);
	CB_BlendSpace2DInfo->UpdateData(&BlendSpace2DData, sizeof(BlendSpace2DInfoDesc));

	const Texture * const AnimTexture1 = Clips[0]->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = Clips[1]->GetKeyFrameTexture();
	const Texture * const AnimTexture3 = Clips[2]->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	CB_BlendSpace2DInfo->BindToGPU();
	AnimTexture1->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture2->BindToGPU(1, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture3->BindToGPU(2, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV

	AnimationBlendSpace2DPlayer->Dispatch();

	const float NextTime = InBlendSpace2D->GetNextFrame(this->CurrentFrame, DeltaSecond);
	if (NextTime > 0)
		this->CurrentFrame = NextTime;
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
		ImGui::SliderFloat("Walk Speed", &WalkSpeed, CurrentBlendSpace->GetMin(), CurrentBlendSpace->GetMax());
		PlayAnimationBlendSpace1D(CurrentBlendSpace, DeltaSecond, WalkSpeed);
	}
	if (!!CurrentBlendSpace2D)
	{
		static float HorizontalSpeed = 0;
		static float VerticalSpeed = 0;
		const array<float, 2> HRange = CurrentBlendSpace2D->GetHorizontalRange();
		const array<float, 2> VRange = CurrentBlendSpace2D->GetVerticalRange();
		ImGui::SliderFloat("Horizontal Speed", &HorizontalSpeed, HRange[0], HRange[1]);
		ImGui::SliderFloat("Vertical Speed", &VerticalSpeed, VRange[0], VRange[1]);
		PlayAnimationBlendSpace2D(CurrentBlendSpace2D, DeltaSecond, HorizontalSpeed, VerticalSpeed);
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

void AnimationController::SetCurrentBlendSpace(AnimationBlendSpace2D * BlendSpace2D)
{
	CurrentAnimation = nullptr;
	CurrentBlendSpace = nullptr;
	CurrentBlendSpace2D = BlendSpace2D;
	BlendSpace2DData = {};
}

AnimationController::AnimationInfoDesc AnimationController::GetInfo(const AnimationClip* Clip, float InCurrentFrame)
{
	const float CurrentFrame = Clip->GetCurrentFrame(InCurrentFrame);
	const int KeyFrameCurr = Clip->GetKeyFrameCurr(CurrentFrame);
	const int KeyFrameNext = Clip->GetKeyFrameNext(CurrentFrame);
	float LerpRate = 0;
	if (KeyFrameNext > 0 && CurrentFrame > (float)KeyFrameCurr)
		LerpRate = (CurrentFrame - (float)KeyFrameCurr) / (float)(KeyFrameNext - KeyFrameCurr);

	return {
		KeyFrameCurr,
		KeyFrameNext,
		LerpRate,
		CurrentFrame
	};
}


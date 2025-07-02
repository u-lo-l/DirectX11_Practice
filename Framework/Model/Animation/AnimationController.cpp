#include "framework.h"
#include "AnimationController.h"

AnimationController::AnimationController(CSkeletal* InSkeletal)
	: TargetSkeletal(InSkeletal)
{
	ASSERT(!!TargetSkeletal, "Skeleton Not Valid");

	CB_AnimationData = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		nullptr,
		"",
		sizeof(Animation_ConstantDesc),
		false
	);

	const UINT BoneCount = TargetSkeletal->GetBoneCount();
	constexpr UINT NumThreadX = 32;
	const UINT DispatchX = BoneCount / NumThreadX + 1;
	const vector<D3D_SHADER_MACRO> Defines = {
		{"THREAD_X", "32"},
		{nullptr, nullptr}
	};
	ComputeShaderDesc Desc = {
		"KeyFrameAnimationCalculator",
		L"Mesh/Animation/AnimationBoneMatrixCalc.hlsl",
		L"",
		Defines.data(),
		L"CSMain",
		32, 1, 1,
		DispatchX, 1, 1,
		{},
		true
	};
	KeyFrameAnimationCalculator = new ComputeShader(Desc);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(KeyFrameAnimationCalculator);
	SAFE_DELETE(CB_AnimationData);
}

void AnimationController::PlaySingleAnimationClip
(
	const AnimationClip * InClip,
	const float DeltaSecond
)
{
	if (!InClip)
		return;

	const AnimationInfoDesc CurrentClipPlayingInfo = GetInfo(InClip, this->NormalizedPlayTime);
	Animation_ConstantData.AnimData[0] = CurrentClipPlayingInfo;
	Animation_ConstantData.AnimData[1] = {};
	Animation_ConstantData.AnimData[2] = {};
	
	CB_AnimationData->UpdateData(&Animation_ConstantData, sizeof(Animation_ConstantDesc));
	
	const Texture * const KeyFrameTexture = InClip->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	KeyFrameAnimationCalculator->BindCB(CB_AnimationData, 0);
	KeyFrameAnimationCalculator->BindSRV(KeyFrameTexture->GetSRV(), 0);
	KeyFrameAnimationCalculator->BindUAV(SB_BoneMatrices->GetUAV(), 0);
	KeyFrameAnimationCalculator->Dispatch();
	
	const float NextTime = InClip->GetNextFrame(this->NormalizedPlayTime, DeltaSecond);
	if (NextTime > 0)
		this->NormalizedPlayTime = NextTime;
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
	
	Animation_ConstantData.AnimData[0] = GetInfo(Anim1, this->NormalizedPlayTime);
	Animation_ConstantData.AnimData[1] = GetInfo(Anim2, this->NormalizedPlayTime);
	Animation_ConstantData.AnimData[2] = {};

	
	CB_AnimationData->UpdateData(&Animation_ConstantData, sizeof(Animation_ConstantDesc));

	const Texture * const AnimTexture1 = Anim1->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = Anim2->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();
	
	KeyFrameAnimationCalculator->BindCB(CB_AnimationData, 0);
	KeyFrameAnimationCalculator->BindSRV(AnimTexture1->GetSRV(), 0);
	KeyFrameAnimationCalculator->BindSRV(AnimTexture2->GetSRV(), 1);
	KeyFrameAnimationCalculator->BindUAV(SB_BoneMatrices->GetUAV(), 0);
	KeyFrameAnimationCalculator->Dispatch();

	const float NextTime = InBlendSpace1D->GetNextFrame(this->NormalizedPlayTime, DeltaSecond);
	if (NextTime > 0)
		this->NormalizedPlayTime = NextTime;
}

void AnimationController::PlayAnimationBlendSpace2D
(
	const AnimationBlendSpace2D * InBlendSpace2D,
	const float DeltaSecond,
	const float ValueHorizontal,
	const float ValueVertical
)
{
	if (KeyFrameAnimationCalculator == nullptr)
		return ;
	if (InBlendSpace2D == nullptr)
		return;
	
	array<const AnimationClip *, 3> SampleClips;
	array<float, 3> Weights;
	InBlendSpace2D->GetTargetAnimations({ValueHorizontal, ValueVertical},SampleClips,Weights);
	ASSERT((Weights[0] > -Math::EPSILON && Weights[1] > -Math::EPSILON && Weights[2] > -Math::EPSILON), "Weight Not Valid : NegativeValue");
	ASSERT(false == (Math::IsZero(Weights[0]) && Math::IsZero(Weights[1]) && Math::IsZero(Weights[2])), "Weight Not Valid : All Zero");
	
	const array<AnimationInfoDesc, 3> Anim1PlayingInfos {
		GetInfo(SampleClips[0], this->NormalizedPlayTime),
		GetInfo(SampleClips[1], this->NormalizedPlayTime),
		GetInfo(SampleClips[2], this->NormalizedPlayTime)
	};

#ifdef DISPLAY_IMGUI_DEBUG_INFO
	ImGui::TextColored(
		{255, 50, 50, 255},
		"Animation Blending, Frame : %.3f | NormalizedTime : %.3f",
		this->NormalizedPlayTime * InBlendSpace2D->GetBlendSpaceLength(),
		this->NormalizedPlayTime
	);
	ImGui::TextColored(
		{255, 50, 50, 255},
		"Anim 1 : %s (%.3f)", SampleClips[0]->GetName().c_str(), Weights[0]
	);
	ImGui::TextColored(
		{255, 50, 50, 255},
		"Anim 2 : %s (%.3f)", SampleClips[1]->GetName().c_str(), Weights[1]
	);
	ImGui::TextColored(
		{255, 50, 50, 255},
		"Anim 3 : %s (%.3f)", SampleClips[2]->GetName().c_str(), Weights[2]
	);
#endif 
	
	Animation_ConstantData = Animation_ConstantDesc(Anim1PlayingInfos, Weights);
	CB_AnimationData->UpdateData(&Animation_ConstantData, sizeof(Animation_ConstantDesc));

	const Texture * const AnimTexture1 = SampleClips[0]->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = SampleClips[1]->GetKeyFrameTexture();
	const Texture * const AnimTexture3 = SampleClips[2]->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	KeyFrameAnimationCalculator->BindCB(CB_AnimationData, 0);
	KeyFrameAnimationCalculator->BindSRV(AnimTexture1->GetSRV(), 0);
	KeyFrameAnimationCalculator->BindSRV(AnimTexture2->GetSRV(), 1);
	KeyFrameAnimationCalculator->BindSRV(AnimTexture3->GetSRV(), 2);
	KeyFrameAnimationCalculator->BindUAV(SB_BoneMatrices->GetUAV(), 0);
	KeyFrameAnimationCalculator->Dispatch();

	float Duration = 0.f;
	for (int i = 0 ; i < 3 ; i++)
		Duration += SampleClips[i]->GetDuration() * Weights[i];
	float PlayRate = InBlendSpace2D->GetDuration() / Duration;
	this->NormalizedPlayTime = InBlendSpace2D->GetNextNormalizedPlayTime(this->NormalizedPlayTime, DeltaSecond * PlayRate);
}

void AnimationController::UpdateAnimationFrameData(float DeltaSecond)
{
	
}

void AnimationController::Tick()
{
	const float DeltaSecond = sdt::SystemTimer::Get()->GetDeltaTime();
	// UpdateAnimationFrameData(DeltaSecond);
	if (!!CurrentAnimation)
	{
		#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::Begin(String::Format("AnimationClip Player %s", CurrentAnimation->GetName().c_str()).c_str());
		#endif
		PlaySingleAnimationClip(CurrentAnimation, DeltaSecond);
		#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::End();
		#endif
		return ;
	}
	if (!!CurrentBlendSpace)
	{
		static float WalkSpeed = 0;
		#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::Begin(String::Format("BlendSpace1D Player %s", CurrentBlendSpace->GetName().c_str()).c_str());
		ImGui::SliderFloat("Walk Speed", &WalkSpeed, CurrentBlendSpace->GetMin(), CurrentBlendSpace->GetMax());
		PlayAnimationBlendSpace1D(CurrentBlendSpace, DeltaSecond, WalkSpeed);
		#endif
		#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::End();
		#endif
		return ;
	}
	if (!!CurrentBlendSpace2D)
	{
		static float LerpRate = 5.f;
#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::Begin(String::Format("BlendSpace2D Player %s", CurrentBlendSpace2D->GetName().c_str()).c_str());
		ImGui::SliderFloat("LerpRate", &LerpRate, 1, 10);
#endif 
		const float Amount = DeltaSecond * LerpRate;
		static float SpeedValue = 0;
		static float HorizontalValue = 0;
		static float VerticalValue = 0;
		
		int ForwardDirection = 0;
		int SideDirection = 0;
		
		ForwardDirection += Keyboard::IsPressed(VK_UP) | Keyboard::IsPressed('W') ? 1 : 0;
		ForwardDirection += Keyboard::IsPressed(VK_DOWN) | Keyboard::IsPressed('S') ? -1 : 0;
		SideDirection += Keyboard::IsPressed(VK_RIGHT) | Keyboard::IsPressed('D') ? 1 : 0;
		SideDirection += Keyboard::IsPressed(VK_LEFT) | Keyboard::IsPressed('A') ? -1 : 0;
		const int GoalSpeed = (ForwardDirection != 0 || SideDirection != 0) && Keyboard::IsPressed(VK_CONTROL) ? 4 : 1;
		
		VerticalValue = Math::Lerp(VerticalValue, static_cast<float>(ForwardDirection), Amount);
		HorizontalValue = Math::Lerp(HorizontalValue, static_cast<float>(SideDirection), Amount);
		SpeedValue = Math::Lerp(SpeedValue, static_cast<float>(GoalSpeed), Amount);
		
		float VerticalSpeed = VerticalValue * SpeedValue;
		float HorizontalSpeed = HorizontalValue * SpeedValue;
#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::SliderFloat("Forward Speed", &VerticalSpeed, -4, 4);
		ImGui::SliderFloat("Rightward Speed", &HorizontalSpeed, -4, 4);
#endif 
		PlayAnimationBlendSpace2D(CurrentBlendSpace2D, DeltaSecond, HorizontalSpeed, VerticalSpeed);
#ifdef DISPLAY_IMGUI_DEBUG_INFO
		ImGui::End();
#endif 
		return ;
	}
}

void AnimationController::SetCurrentAnimation(AnimationClip * Clip)
{
	CurrentBlendSpace = nullptr;
	CurrentAnimation = Clip;
	Animation_ConstantData = {};
}

void AnimationController::SetCurrentBlendSpace(AnimationBlendSpace1D* BlendSpace1D)
{
	CurrentAnimation = nullptr;
	CurrentBlendSpace = BlendSpace1D;
	Animation_ConstantData = {};
}

void AnimationController::SetCurrentBlendSpace(AnimationBlendSpace2D * BlendSpace2D)
{
	CurrentAnimation = nullptr;
	CurrentBlendSpace = nullptr;
	CurrentBlendSpace2D = BlendSpace2D;
	Animation_ConstantData = {};
}

AnimationController::AnimationInfoDesc AnimationController::GetInfo(const AnimationClip* Clip, float InNormalizedPlayTime)
{
	const float CurrentPlayTime = Clip->GetCurrentPlayTime(InNormalizedPlayTime);
	const int KeyFrameCurr = Clip->GetKeyFrameCurr(CurrentPlayTime);
	const int KeyFrameNext = Clip->GetKeyFrameNext(CurrentPlayTime);
	float LerpRate = 0;
	if (KeyFrameNext > 0 && CurrentPlayTime > (float)KeyFrameCurr)
		LerpRate = (CurrentPlayTime - (float)KeyFrameCurr) / (float)(KeyFrameNext - KeyFrameCurr);

	return {
		KeyFrameCurr,
		KeyFrameNext,
		LerpRate,
		CurrentPlayTime
	};
}
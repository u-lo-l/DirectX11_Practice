#include "framework.h"
#include "AnimationController.h"

AnimationController::AnimationController(CSkeletal* InSkeletal)
	: TargetSkeletal(InSkeletal)
{
	ASSERT(!!TargetSkeletal, "Skeleton Not Valid");

	CB_AnimationData = new ConstantBuffer(
		static_cast<UINT>(ShaderType::ComputeShader),
		0,
		nullptr,
		"",
		sizeof(Animation_ConstantDesc),
		false
	);

	const vector<D3D_SHADER_MACRO> Defines = {
		{"THREAD_X", "32"},
		{nullptr, nullptr}
	};
	AnimationBoneTransformCalculator = new HlslComputeShader(
		L"Mesh/Animation/AnimationBoneMatrixCalc.hlsl",
		Defines.data(),
		"CSMain",
		true
	);
	AnimationBoneTransformCalculator->SetDispatchSize(8, 1, 1);
}

AnimationController::~AnimationController()
{
	SAFE_DELETE(AnimationBoneTransformCalculator);
	// SAFE_DELETE(AnimationKeyFrameBlender);
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

	CB_AnimationData->BindToGPU();
	KeyFrameTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV
	
	AnimationBoneTransformCalculator->Dispatch();
	
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
	
	CB_AnimationData->BindToGPU();
	AnimTexture1->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture2->BindToGPU(1, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV

	AnimationBoneTransformCalculator->Dispatch();

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
	if (AnimationBoneTransformCalculator == nullptr)
		return ;
	if (!InBlendSpace2D)
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

	Gui::Get()->RenderText(5, 150, 1.f, 0.2f, 0.2f,
		String::Format("Animation Blending, Frame : %.3f | NormalizedTime : %.3f",
			this->NormalizedPlayTime * InBlendSpace2D->GetBlendSpaceLength(),
			this->NormalizedPlayTime
		)
	);
	Gui::Get()->RenderText(5, 170, 1.f, 0.2f, 0.2f,
		String::Format("Anim 1 : %s (%.3f)", SampleClips[0]->GetName().c_str(), Weights[0])
	);
	Gui::Get()->RenderText(5, 190, 1.f, 0.2f, 0.2f,
		String::Format("Anim 2 : %s (%.3f)", SampleClips[1]->GetName().c_str(), Weights[1])
	);
	Gui::Get()->RenderText(5, 210, 1.f, 0.2f, 0.2f,
		String::Format("Anim 3 : %s (%.3f)", SampleClips[2]->GetName().c_str(), Weights[2])
	);
	Animation_ConstantData = Animation_ConstantDesc(Anim1PlayingInfos, Weights);
	CB_AnimationData->UpdateData(&Animation_ConstantData, sizeof(Animation_ConstantDesc));

	const Texture * const AnimTexture1 = SampleClips[0]->GetKeyFrameTexture();
	const Texture * const AnimTexture2 = SampleClips[1]->GetKeyFrameTexture();
	const Texture * const AnimTexture3 = SampleClips[2]->GetKeyFrameTexture();
	RWStructuredBuffer * const SB_BoneMatrices = TargetSkeletal->GetBoneMatrices_Buffer();

	CB_AnimationData->BindToGPU();
	AnimTexture1->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture2->BindToGPU(1, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	AnimTexture3->BindToGPU(2, static_cast<UINT>(ShaderType::ComputeShader)); //SRV
	SB_BoneMatrices->BindToGPUAsUAV(0); //UAV

	AnimationBoneTransformCalculator->Dispatch();

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
		static float LerpRate = 5.f;
		ImGui::SliderFloat("LerpRate", &LerpRate, 1, 10);
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
		ImGui::SliderFloat("Forward Speed", &VerticalSpeed, -4, 4);
		ImGui::SliderFloat("Rightward Speed", &HorizontalSpeed, -4, 4);
		PlayAnimationBlendSpace2D(CurrentBlendSpace2D, DeltaSecond, HorizontalSpeed, VerticalSpeed);
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


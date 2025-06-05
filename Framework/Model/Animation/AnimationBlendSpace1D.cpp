#include "framework.h"
#include "AnimationBlendSpace1D.h"

#include "AnimationClip.h"

AnimationBlendSpace1D::AnimationBlendSpace1D(const CSkeletal* InSkeleton)
{
	this->Skeleton = InSkeleton;
}

AnimationBlendSpace1D::~AnimationBlendSpace1D()
{
	Animations.clear();
}

void AnimationBlendSpace1D::SetHorizontalRange(float Value1, float Value2)
{
	Min = Value1;
	Max = Value2;
}

void AnimationBlendSpace1D::AddAnimation(AnimationClip* Anim, float At)
{
	const auto & It = Animations.find(At);
	if (It != Animations.cend())
	{
		float RemoveClipDuration = It->second->GetDuration();
		if (this->Duration == RemoveClipDuration)
			RefreshDuration();
	}
	Animations[At] = Anim;
	Duration = max(Duration, Anim->GetDuration());
}

void AnimationBlendSpace1D::Play(float Value, float Time)
{
	
}

float AnimationBlendSpace1D::GetDuration() const
{
	return Duration;
}

float AnimationBlendSpace1D::GetBlendSpaceLength() const
{
	return Duration + 1;
}

void AnimationBlendSpace1D::GetTargetAnimations
(
	float Value,
	const AnimationClip ** OutAnim1,
	const AnimationClip ** OutAnim2,
	float * Alpha
) const
{
	if (Animations.empty())
	{
		*OutAnim1 = *OutAnim2 = nullptr;
		return ;
	}
	if (Max == Min)
	{
		Value = Max;
		*Alpha = 0;
	}
	else if (bWrap)
	{
		Value = WrapValue(Value);
		*Alpha = (Value - Min) / (Max - Min);
	}
	else
	{
		Value = Math::Clamp(Value, this->Min, this->Max);
		*Alpha = (Value - Min) / (Max - Min);
	}

	const auto It2 = Animations.upper_bound(Value);
	if (It2 == Animations.cbegin())
	{
		*OutAnim1 = *OutAnim2 = It2->second;
	}
	else if (It2 == Animations.cend())
	{
		const auto It1 = std::prev(It2);
		*OutAnim1 = *OutAnim2 = It1->second;
	}
	else
	{
		const auto It1 = std::prev(It2);
		*OutAnim1 = It1->second;
		*OutAnim2 = It2->second;
	}
}

float AnimationBlendSpace1D::GetNextFrame(float CurrentFrame, float DeltaSecond) const
{
	const float BlendSpaceFullTime = GetBlendSpaceLength() * 30;
	const float DeltaFrame = DeltaSecond * 30.f * 1.f ;
	CurrentFrame += DeltaFrame;
	return fmod(CurrentFrame, BlendSpaceFullTime);
}

void AnimationBlendSpace1D::SetWrapped(bool bWrapped)
{
	bWrap = bWrapped;
}

float AnimationBlendSpace1D::WrapValue(const float InValue) const
{
	ASSERT(Max >= Min, String::Format("%s Range Not Valid", __FUNCTION__).c_str())
	const float Range = Max - Min;
	if (Range < Math::Epsilon)
	{
		return InValue;
	}
	if (InValue < Min)
	{
		float Gap = fmod(Min - InValue, Range);
		return Max - Gap;
	}
	if (InValue >= Max)
	{
		float Gap = fmod(InValue - Max, Range);
		return Min + Gap;
	}
	return InValue;
}

void AnimationBlendSpace1D::RefreshDuration()
{
	float Max = -1.f;
	for (const auto & Pair : Animations)
		Max = max(Max, Pair.second->GetDuration());
	Duration = Max;
}

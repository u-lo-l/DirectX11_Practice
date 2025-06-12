#include "framework.h"
#include "AnimationBlendSpace2D.h"

AnimationBlendSpace2D::AnimationBlendSpace2D(const CSkeletal* InSkeleton)
	: Skeleton(InSkeleton), Triangulator({})
{
}

AnimationBlendSpace2D::~AnimationBlendSpace2D()
= default;

void AnimationBlendSpace2D::SetHorizontalRange(float Value1, float Value2)
{
	ASSERT(false == Math::NearEqual(Value1, Value2), "SetHorizontalRange | Range To Narrow");
	HorizontalRange = {Value1, Value2};
}

void AnimationBlendSpace2D::SetVerticalRange(float Value1, float Value2)
{
	ASSERT(false == Math::NearEqual(Value1, Value2), "SetVerticalRange | Range To Narrow");
	VerticalRange = {Value1, Value2};
}

const array<float, 2>& AnimationBlendSpace2D::GetHorizontalRange() const
{
	return HorizontalRange;
}

const array<float, 2>& AnimationBlendSpace2D::GetVerticalRange() const
{
	return VerticalRange;
}

void AnimationBlendSpace2D::AddAnimation(const AnimationClip * Anim, const array<float, 2> & At)
{
	if (HorizontalRange[0] == NAN || HorizontalRange[1] == NAN || VerticalRange[0] == NAN || VerticalRange[1] == NAN)
		ASSERT(false, "BlendSpace2D Range Not Valid");
	
	float HorizontalValue = NormalizeHorizontalValue(At[HORIZONTAL]);
	float VerticalValue = NormalizeVerticalValue(At[VERTICAL]);
	
	Triangulator.AddSample(Anim, {HorizontalValue, VerticalValue});
	Duration = max(Duration, Anim->GetDuration());
}

void AnimationBlendSpace2D::EndAddingAnimation()
{
	Triangulator.Triangulate();
}

float AnimationBlendSpace2D::GetDuration() const
{
	return Duration;
}

float AnimationBlendSpace2D::GetBlendSpaceLength() const
{
	return Duration + 1;
}

void AnimationBlendSpace2D::SetVerticalWrapped(bool bWrapped)
{
	bWrapVertical = bWrapped;
}

void AnimationBlendSpace2D::SetHorizontalWrapped(bool bWrapped)
{
	bWrapHorizontal = bWrapped;
}

float AnimationBlendSpace2D::GetNextFrame(float CurrentFrame, float DeltaSecond) const
{
	const float BlendSpaceFullTime = GetBlendSpaceLength() * 30;
	const float DeltaFrame = DeltaSecond * 30.f * 1.f ;
	CurrentFrame += DeltaFrame;
	return fmod(CurrentFrame, BlendSpaceFullTime);
}

void AnimationBlendSpace2D::GetTargetAnimations
(
	const array<float, 2> & Value,
	array<const AnimationClip*, 3> & OutClips,
	array<float, 3> & BarycentricWeights
) const
{
	const Vector2D Position = {
		NormalizeHorizontalValue(Value[0]),
		NormalizeVerticalValue(Value[1])
	};
	bool Result = Triangulator.GetAnimsAndWeights(Position, OutClips, BarycentricWeights);
	if (Result == false)
		ASSERT(false, String::Format("%s | Triangle Not Found", __FUNCTION__).c_str());
}

float AnimationBlendSpace2D::NormalizeValue
(
	float InValue,
	const array<float, 2> & Range,
	bool bWrap
)
{
	if (Math::NearEqual(Range[0], Range[1]))
		return Range[0];

	const float RangeLengthInv = 1 / (Range[1] - Range[0]);
	InValue = (InValue - Range[0]) * RangeLengthInv;

	return bWrap ? Math::Wrap01(InValue) : Math::Clamp01(InValue);
}

float AnimationBlendSpace2D::NormalizeHorizontalValue(float InValue) const
{
	return NormalizeValue(InValue, HorizontalRange, bWrapHorizontal);
}

float AnimationBlendSpace2D::NormalizeVerticalValue(float InValue) const
{
	return NormalizeValue(InValue, VerticalRange, bWrapVertical);
}

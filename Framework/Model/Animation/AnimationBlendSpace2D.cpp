#include "framework.h"
#include "AnimationBlendSpace2D.h"

AnimationBlendSpace2D::AnimationBlendSpace2D(const CSkeletal* InSkeleton)
	: Skeleton(InSkeleton), Triangulator({})
{
}

AnimationBlendSpace2D::~AnimationBlendSpace2D()
{
}

void AnimationBlendSpace2D::SetHorizontalRange(float Value1, float Value2)
{
	HorizontalRange = {Value1, Value2};
}

void AnimationBlendSpace2D::SetVerticalRange(float Value1, float Value2)
{
	VerticalRange = {Value1, Value2};
}

void AnimationBlendSpace2D::AddAnimation(const AnimationClip * Anim, const array<float, 2> & At)
{
	const Vector2D Position = Vector2D(At[0], At[1]);
	SamplePositions.push_back(Position);
	Samples[Position] = Anim;
	Triangulator.AddSample({At[0], At[1]});
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
	const Vector2D Position = {Value[0], Value[1]};
	array<Vector2D, 3> TriangleVertices;
	if (Triangulator.GetTriangle(Position, TriangleVertices, BarycentricWeights) == false)
		ASSERT(false, String::Format("%s | Triangle Not Found", __FUNCTION__).c_str());
	
	for (int i = 0 ; i < 3; i++)
	{
		const auto & It = Samples.find(TriangleVertices[i]);
		if (It != Samples.cend())
			OutClips[i] = It->second;
		else
			ASSERT(false, String::Format("%s | Sample Not Found", __FUNCTION__).c_str());
	}

}

void AnimationBlendSpace2D::NormalizeValue
(
	float InValue,
	const array<float, 2> & Range,
	bool bWrap
)
{
}

void AnimationBlendSpace2D::NormalizeHorizontalValue(float InValue)
{
}

void AnimationBlendSpace2D::NormalizeVerticalValue(float InValue)
{
}

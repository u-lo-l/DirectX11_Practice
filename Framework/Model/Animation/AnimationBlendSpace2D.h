#pragma once

class AnimationClip;

class AnimationBlendSpace2D
{
private:
	struct AnimationSample
	{
		AnimationSample(const AnimationClip * InClip, const Vector2D & SamplePosition)
			: Clip(InClip), Position(SamplePosition) {}
		const AnimationClip * Clip;
		Vector2D Position;
	};
	struct BlendSpaceTriangle
	{
		static constexpr int NUM_VERTICES = 3;
		array<Vector2D, NUM_VERTICES> SampleVertices;
		array<int, NUM_VERTICES> SampleIndices;
	};
public:
	explicit AnimationBlendSpace2D(const CSkeletal * InSkeleton);
	~AnimationBlendSpace2D();

	void SetHorizontalRange(float Value1, float Value2);
	void SetVerticalRange(float Value1, float Value2);
	void AddAnimation(const AnimationClip * Anim, const array<float, 2> & At);
	float GetDuration() const;
	float GetBlendSpaceLength() const;

	void SetVerticalWrapped(bool bWrapped);
	void SetHorizontalWrapped(bool bWrapped);
	float GetNextFrame(float CurrentFrame, float DeltaSecond) const;

	void GetTargetAnimations(
		const array<float, 2> & Value,
		array<const AnimationClip*, 3> & OutClips,
		array<float, 3> & BarycentricWeights
	) const;
private:
	void NormalizeValue(float InValue, const array<float, 2> & Range, bool bWrap);
	void NormalizeHorizontalValue(float InValue);
	void NormalizeVerticalValue(float InValue);
	const CSkeletal * Skeleton;
	float Duration = -1.f;
	float PlayRate = 1.f;
	array<float, 2> HorizontalRange = {0, 0};
	array<float, 2> VerticalRange = {0, 0};;
	bool bWrapHorizontal = false;
	bool bWrapVertical = false;
	
	vector<Vector2D> SamplePositions;
	unordered_map<Vector2D, const AnimationClip *> Samples;
	DelaunayTriangulator2D Triangulator;
};

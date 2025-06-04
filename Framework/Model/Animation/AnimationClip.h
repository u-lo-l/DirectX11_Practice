#pragma once
#include "AnimationController.h"

class AnimationClip
{
private:
	struct KeyFrameData
	{
		int BoneIndex = -1;
		string BoneName;
		vector<FrameDataVec>  Positions;
		vector<FrameDataVec>  Scales;
		vector<FrameDataQuat> Rotations;
	};
public:
	
	AnimationClip(const CSkeletal * InSkeleton, const wstring & AnimationAssetPath);
	~AnimationClip();
	const string & GetName() const;
	float GetPlayRate() const;
	float GetDuration() const;
	float GetAnimationLength () const { return 1 + Duration; }
	float GetTickPerSecond() const;
	void SetLoop(bool bLoop);
	bool IsLoop() const;
	const Texture * GetKeyFrameTexture() const;

	float CalculateNextAnimTime(float CurrentTime, float DeltaSecond) const;
private:
	void ReadAnimationAsset(
		const CSkeletal* InSkeleton,
		const wstring& AnimationAssetPath,
		vector<KeyFrameData*> & OutKeyFrames
	);
	void CreateKeyFrameTable(
		const CSkeletal* InSkeleton,
		const vector<KeyFrameData*>& InKeyFrames, vector<Matrix>& OutKeyFrameTable
	);
	void CreateKeyFrameTexture(const vector<Matrix>& InKeyFramesArray);
	string Name;
	float Duration;
	float TickPerSecond;
	Texture * KeyFrameTexture;
	const CSkeletal * Skeleton = nullptr;
	bool bLoop = false;
	float PlayRate = 1.f;
};

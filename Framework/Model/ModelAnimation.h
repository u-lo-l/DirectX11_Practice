#pragma once
#include "Core/Quaternion.h"
// #include "Model.h"

typedef struct ModelAnimationFrameVector FrameDataVec;
typedef struct ModelAnimationFrameQuat FrameDataQuat;

class ModelAnimation
{
private :
	friend class Model;
	struct KeyFrameData;
	ModelAnimation() = default;
	~ModelAnimation();
private:
	static ModelAnimation * ReadAnimationFile(
		const BinaryReader * InReader,
		const Model::CachedBoneTableType * InCachedBoneTable
	);
private:
	UINT GetKeyFrameCount() const { return KeyFrames.size(); }
	string Name;
	float Duration = 0.f;
	float TicksPerSecond = 0.f;;
	vector<KeyFrameData *> KeyFrames;

private :
	ClipTransform * CalcClipTransform( const vector<ModelBone *> & InBone ) const;
	
private:
	struct KeyFrameData
	{
		explicit KeyFrameData(int InBoneIndex = -1, string InBoneName = "");
		~KeyFrameData();
		
		int BoneIndex = -1;
		string BoneName;

		vector<FrameDataVec> Positions;
		vector<FrameDataVec> Scales;
		vector<FrameDataQuat> Rotations;
	};
	struct ClipTransform
	{
		Matrix** TransformMats;
		ClipTransform();
		~ClipTransform();
	};
};

// KeyFrame Animation의 Transform정보에서 Translation와 Scale에 대한 정보를 담을 구조체
struct ModelAnimationFrameVector
{
	ModelAnimationFrameVector() = default;
	ModelAnimationFrameVector(float InTime, const Vector& InValue)
		: Time(InTime), Value(InValue) {}
	float Time = 0;
	Vector Value;
};

// KeyFrame Animation의 Transform정보에서 Rotation에 대한 정보를 담을 구조체
struct ModelAnimationFrameQuat
{
	ModelAnimationFrameQuat() = default;
	ModelAnimationFrameQuat(float InTime, const Quaternion& InValue)
		: Time(InTime), Value(InValue) {}
	float Time = 0;
	Quaternion Value;
};
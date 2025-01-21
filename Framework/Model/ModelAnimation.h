#pragma once
#include "Core/Quaternion.h"
// #include "Model.h"

typedef struct ModelAnimationFrameVector FrameDataVec;
typedef struct ModelAnimationFrameQuat FrameDataQuat;

class ModelAnimation
{
public:
	static constexpr UINT MaxFrameLength = 512;
private :
	friend class Model;
	struct KeyFrameData;
	struct KeyFrameTFTable;
	ModelAnimation() = default;
	~ModelAnimation();
private:
	static ModelAnimation * ReadAnimationFile(
		const BinaryReader * InReader,
		const Model::CachedBoneTableType * InCachedBoneTable
	);
private:
	UINT GetKeyFrameCount() const { return KeyFrames.size(); }
	float GetAnimationLength () const { return 1 + Duration; }
	string Name;
	float Duration = 0.f;
	float TicksPerSecond = 0.f;;
	vector<KeyFrameData *> KeyFrames;

private :
	KeyFrameTFTable *  CalcClipTransform( const vector<ModelBone *> & InBone) const;
	
private:
	// 어떤 특정 Bone에 연결된 Key 정보들?
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
	
	struct KeyFrameTFTable // ClipTransform
	{
		Matrix** TransformMats;
		KeyFrameTFTable();
		~KeyFrameTFTable();
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
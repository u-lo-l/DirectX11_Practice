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
	~ModelAnimation() = default;
private:
	static ModelAnimation * ReadAnimationFile(
		const BinaryReader * InReader,
		const Model::CachedBoneTableType * InCachedBoneTable
	);
private:
	string Name;
	float Duration = 0.f;
	float TicksPerSecond = 0.f;;
	vector<KeyFrameData> KeyFrames;
	
private:
	struct KeyFrameData
	{
		explicit KeyFrameData(int InBoneIndex = -1, string InBoneName = "");
		~KeyFrameData();
		
		int BoneIndex = -1;
		string BoneName;

		// 왜 여기선 vector<T> 안 쓰고 pointer 쓰나요?
		// ReadByte로 읽어올 거라 어차피 포인터 필요함. 굳이 vector로 옮길 필요 없음.
		UINT PosCount = 0;
		FrameDataVec * Positions;

		UINT RotCount = 0;
		FrameDataQuat * Rotations;

		UINT ScaleCount = 0;
		FrameDataVec * Scales;
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
#include "framework.h"
#include "ModelAnimation.h"

ModelAnimation * ModelAnimation::ReadAnimationFile(
	const BinaryReader * InReader,
	const Model::CachedBoneTableType * InCachedBoneTable
)
{
	ModelAnimation * ModelAnimationToReturn = new ModelAnimation();

	ModelAnimationToReturn->Name = InReader->ReadString();
	ModelAnimationToReturn->Duration = InReader->ReadFloat();
	ModelAnimationToReturn->TicksPerSecond = InReader->ReadFloat();

	const UINT NodeCount = InReader->ReadUint();
	ModelAnimationToReturn->KeyFrames.resize(NodeCount);
	for (UINT i = 0; i < NodeCount; i++)
	{
		KeyFrameData & TargetData = ModelAnimationToReturn->KeyFrames[i];
		const string StringToCompare = InReader->ReadString();
		const auto Iterator = InCachedBoneTable->find(StringToCompare);
		if (Iterator != InCachedBoneTable->cend())
		{
			TargetData.BoneIndex = Iterator->second->Index;
			TargetData.BoneName = Iterator->second->Name;
		}
		else
		{
			TargetData.BoneIndex = -1;
			TargetData.BoneName = "";
		}
		
		TargetData.PosCount = InReader->ReadUint();
		if (TargetData.PosCount > 0)
		{
			TargetData.Positions = new FrameDataVec[TargetData.PosCount];
			const UINT ReadSize = sizeof(FrameDataVec) * TargetData.PosCount;
			InReader->ReadByte(reinterpret_cast<void **>(&(TargetData.Positions)), ReadSize);
		}

		TargetData.ScaleCount = InReader->ReadUint();
		if (TargetData.ScaleCount > 0)
		{
			TargetData.Scales = new FrameDataVec[TargetData.ScaleCount];
			const UINT ReadSize = sizeof(FrameDataVec) * TargetData.ScaleCount;
			InReader->ReadByte(reinterpret_cast<void **>(&(TargetData.Scales)), ReadSize);
		}

		TargetData.RotCount = InReader->ReadUint();
		if (TargetData.RotCount > 0)
		{
			TargetData.Rotations = new FrameDataQuat[TargetData.RotCount];
			const UINT ReadSize = sizeof(FrameDataQuat) * TargetData.RotCount;
			InReader->ReadByte(reinterpret_cast<void **>(&(TargetData.Rotations)), ReadSize);
		}
	}
	return ModelAnimationToReturn;
}

/*=================================================================================================*/

ModelAnimation::KeyFrameData::KeyFrameData( int InBoneIndex, string InBoneName )
	: BoneIndex(InBoneIndex)
	, BoneName(std::move(InBoneName))
	, Positions(nullptr)
	, Rotations(nullptr)
	, Scales(nullptr)
{
}

ModelAnimation::KeyFrameData::~KeyFrameData()
{
#ifdef DO_DEBUG
	printf("KeyFrameData Destruct : [%d], <%s>\n", BoneIndex, BoneName.c_str());
#endif
	SAFE_DELETE_ARR(Positions);
	SAFE_DELETE_ARR(Rotations);
	SAFE_DELETE_ARR(Scales);
}

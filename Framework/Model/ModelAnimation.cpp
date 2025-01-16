#include "framework.h"
#include "ModelAnimation.h"

ModelAnimation::~ModelAnimation()
{
	for (const KeyFrameData * Data : KeyFrames)
		SAFE_DELETE(Data);
}

ModelAnimation * ModelAnimation::ReadAnimationFile(
	const BinaryReader * InReader,
	const Model::CachedBoneTableType * InCachedBoneTable
)
{
	ModelAnimation * ModelAnimationToReturn = new ModelAnimation();

	ModelAnimationToReturn->Name = InReader->ReadString();
	ModelAnimationToReturn->Duration = InReader->ReadFloat();
	ModelAnimationToReturn->TicksPerSecond = InReader->ReadFloat();

	const UINT KeyFrameCount = InReader->ReadUint();
	ModelAnimationToReturn->KeyFrames.assign(KeyFrameCount, nullptr);
	for (UINT i = 0; i < KeyFrameCount; i++)
	{
		ModelAnimationToReturn->KeyFrames[i] = new KeyFrameData();
		KeyFrameData * &TargetData = ModelAnimationToReturn->KeyFrames[i];
		const string StringToCompare = InReader->ReadString();
		const auto Iterator = InCachedBoneTable->find(StringToCompare);
		if (Iterator != InCachedBoneTable->cend())
		{
			TargetData->BoneIndex = Iterator->second->Index;
			TargetData->BoneName = Iterator->second->Name;
		}
		else
		{
			// 이 친구들은 버린다?
		}
		
		const UINT PosCount = InReader->ReadUint();
		TargetData->Positions.resize(PosCount);
		if (PosCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * PosCount;
			void * Ptr = TargetData->Positions.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT ScaleCount = InReader->ReadUint();
		TargetData->Scales.resize(ScaleCount);
		if (ScaleCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * ScaleCount;
			void * Ptr = TargetData->Scales.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT RotCount = InReader->ReadUint();
		TargetData->Rotations.resize(RotCount);
		if (RotCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataQuat) * RotCount;
			void * Ptr = TargetData->Rotations.data();
			InReader->ReadByte(&Ptr, ReadSize);
		}
	}
	return ModelAnimationToReturn;
}

/*=================================================================================================*/

ModelAnimation::KeyFrameData::KeyFrameData( int InBoneIndex, string InBoneName )
	: BoneIndex(InBoneIndex)
	, BoneName(std::move(InBoneName))
{
}

ModelAnimation::KeyFrameData::~KeyFrameData()
{
#ifdef DO_DEBUG
	printf("KeyFrameData Destruct : [%d], <%s>\n", BoneIndex, BoneName.c_str());
#endif
}

ModelAnimation::ClipTransform::ClipTransform()
{
	TransformMats = new Matrix* [MaxFrameLength];

	for (UINT i = 0; i < MaxFrameLength; i++)
		TransformMats[i] = new Matrix[ModelMesh::MaxBoneTFCount];
}

ModelAnimation::ClipTransform::~ClipTransform()
{
	for (UINT i = 0; i < MaxFrameLength; i++)
		SAFE_DELETE_ARR(TransformMats[i]);
	SAFE_DELETE_ARR(TransformMats);
}

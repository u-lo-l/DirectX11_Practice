#include "framework.h"
#include "ModelBone.h"
#include "Skeleton.h"

ModelBone::ModelBone()
{
}

ModelBone::~ModelBone()
{
}

void ModelBone::ReadModelFile( const BinaryReader * InReader, Skeleton* &OutSkeleton )
{
	OutSkeleton = nullptr;
	const UINT BoneCount = InReader->ReadUint();
	if (BoneCount == 0)
		return ;
	OutSkeleton = new Skeleton();
#ifdef DO_DEBUG
	printf("Bone Count : %d\n", BoneCount);
#endif
	vector<ModelBone *> & TargetBones = OutSkeleton->Bones;
	TargetBones.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; i++)
	{
		TargetBones[i] = new ModelBone();
		TargetBones[i]->Index = InReader->ReadUint();
		TargetBones[i]->Name = InReader->ReadString();
		TargetBones[i]->ParentIndex = InReader->ReadInt();
		TargetBones[i]->Transform = InReader->ReadMatrix();

		const UINT MeshCount = InReader->ReadUint();
		vector<UINT> & Numbers = TargetBones[i]->MeshIndices;
		Numbers.resize(MeshCount);

		if (MeshCount > 0)
		{
			Numbers.assign(MeshCount, 0);
			void * Ptr = &(Numbers[0]);
			InReader->ReadByte(&Ptr, sizeof(UINT) * MeshCount);
		}
	}

	for (ThisClassPtr Bone : TargetBones)
	{
		if (Bone->IsRootBone() == true) // root bone-node
			continue;;
		
		Bone->Parent = TargetBones[Bone->ParentIndex];
		Bone->Parent->Children.push_back(Bone);
	}
}


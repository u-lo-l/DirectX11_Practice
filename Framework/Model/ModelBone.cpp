#include "framework.h"
#include "ModelBone.h"

ModelBone::ModelBone()
{
}

ModelBone::~ModelBone()
{
}

void ModelBone::ReadFile( const BinaryReader * InReader, vector<ThisClassPtr> & OutBones )
{
	UINT BoneCount = InReader->ReadUint();
	OutBones.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; i++)
	{
		OutBones[i] = new ThisClass();
		OutBones[i]->Index = InReader->ReadUint();
		OutBones[i]->Name = InReader->ReadString();
		OutBones[i]->ParentIndex = InReader->ReadInt();
		OutBones[i]->Transform = InReader->ReadMatrix();

		const UINT MeshCount = InReader->ReadUint();
		vector<UINT> & Numbers = OutBones[i]->MeshIndices;
		Numbers.resize(MeshCount);

		if (MeshCount > 0)
		{
			Numbers.assign(MeshCount, 0);
			void * Ptr = &(Numbers[0]);
			InReader->ReadByte(&Ptr, sizeof(UINT) * MeshCount);
		}
	}
}

#include "framework.h"
#include "Skeleton.h"

Skeleton::Skeleton()
	: RootBone(nullptr)
{
}

Skeleton::~Skeleton()
{
	for (const ModelBone * Bone : Bones)
		SAFE_DELETE(Bone);
}

void Skeleton::SetUpBoneTable(const vector<ModelMesh *> & Meshes)
{
	this->CachedBoneTable = new CachedBoneTableType(); // 애니메이션 다 읽으면 지워진다.
	for (int i = 0; i < GetBoneCount(); i++)
	{
		ModelBone * TargetBone = this->Bones[i];
		this->BoneTransforms[i] = Matrix::Invert(TargetBone->Transform);
		for (const UINT number : TargetBone->MeshIndices)
		{
			SkeletalMesh * SkMesh = dynamic_cast<SkeletalMesh*>(Meshes[number]);
			if (SkMesh == nullptr)
				continue;
			SkMesh->SetBoneIndex(TargetBone->Index);
			SkMesh->Bone = TargetBone;
			SkMesh->SetBoneTransforms(this->BoneTransforms);
		}
		(*CachedBoneTable)[TargetBone->Name] = TargetBone;
	}
}

void Skeleton::ClearBoneTable()
{
	SAFE_DELETE(CachedBoneTable);
}

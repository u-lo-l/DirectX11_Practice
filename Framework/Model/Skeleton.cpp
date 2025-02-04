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

		/*
		 * 기본적으로 mVertex들은 fbx모델의 Root를 기준으로 한다.
		 * 하지만 mMesh가 어떤 aiNode의 Transform을 참조하는경우 해당 Transform(이하 aiTF)을 기준으로한다.
		 * 여기서 문제는 하나의 mMesh가 여러 aiNode로부터 참조될 수 있기에 그냥 초기에 변환해버리면 안된다는 것이다.
		 * 최종적인 Root기준의 mVertex는 
		 */
		this->OffsetMatrix[i] = Matrix::Invert(TargetBone->Transform);
		this->BoneTransform[i] = TargetBone->Transform;
		for (const UINT number : TargetBone->MeshIndices)
		{
			SkeletalMesh * SkMesh = dynamic_cast<SkeletalMesh*>(Meshes[number]);
			if (SkMesh == nullptr)
				continue;
			SkMesh->SetBoneIndex(TargetBone->Index);
			SkMesh->Bone = TargetBone;
			SkMesh->SetOffsetMatrix(this->OffsetMatrix);
			SkMesh->SetBoneTransforms(this->BoneTransform);
		}
		(*CachedBoneTable)[TargetBone->Name] = TargetBone;
	}
}

void Skeleton::ClearBoneTable()
{
	SAFE_DELETE(CachedBoneTable);
}

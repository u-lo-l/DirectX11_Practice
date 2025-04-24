#include "framework.h"
#include "Skeleton.h"

Skeleton::Skeleton()
	: BoneDescBuffer(nullptr)
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

		this->BoneData.OffsetMatrix[i] = Matrix::Invert(TargetBone->Transform, true);
		this->BoneData.BoneTransform[i] = TargetBone->Transform; // Bone의 Root기준 Transform.
		for (const UINT number : TargetBone->MeshIndices)
		{
			SkeletalMesh * SkMesh = dynamic_cast<SkeletalMesh*>(Meshes[number]);
			if (SkMesh == nullptr)
				continue;
			SkMesh->SetBoneIndex(TargetBone->Index);
		}
		(*CachedBoneTable)[TargetBone->Name] = TargetBone;
	}
	// 이것 이후에 CreateBuffer 해야함.
}

void Skeleton::ClearBoneTable()
{
	SAFE_DELETE(CachedBoneTable);
}

void Skeleton::CreateBuffer()
{
	const string CBufferInfo = "Bone Transform Data";
	BoneDescBuffer = new ConstantBuffer(
		ShaderType::VertexShader,
		VS_BoneMatrix,
		&BoneData,
		CBufferInfo,
		sizeof(BoneDesc),
		true
	);
}

void Skeleton::BindToGPU() const
{
	BoneDescBuffer->BindToGPU();
}

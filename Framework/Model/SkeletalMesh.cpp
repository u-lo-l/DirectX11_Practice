#include "framework.h"
#include "SkeletalMesh.h"

SkeletalMesh::SkeletalMesh()
	: BoneIndexData(), CB_BoneIndex(nullptr)
{
}

SkeletalMesh::~SkeletalMesh()
{
#pragma region Bone
	SAFE_DELETE(CB_BoneIndex);
#pragma endregion Bone
}

void SkeletalMesh::Render(UINT InstanceCount) const
{
	if (CB_BoneIndex != nullptr)
		CB_BoneIndex->BindToGPU();

	SubMesh::Render(InstanceCount);
}

void SkeletalMesh::CreateBuffers()
{
	SubMesh::CreateBuffers();
	const string CBufferInfo = MeshName + " : Base Bone Index for this Mesh";
	CB_BoneIndex = new ConstantBuffer(
		ShaderType::VertexShader,
		VS_BoneIndex,
		&BoneIndexData,
		CBufferInfo,
		sizeof(BoneIndexDesc),
		true
	);
}

int SkeletalMesh::GetBoneIndex() const
{
	return static_cast<int>(BoneIndexData.BaseBoneIndex);
}
void SkeletalMesh::SetBoneIndex(int InBoneIndex)
{
	BoneIndexData.BaseBoneIndex = InBoneIndex;
}

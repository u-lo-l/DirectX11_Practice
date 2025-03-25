#include "framework.h"
#include "SkeletalMesh.h"

SkeletalMesh::SkeletalMesh()
	: BoneIndexData(), BoneIndexBuffer(nullptr)
{
}

SkeletalMesh::~SkeletalMesh()
{
#pragma region Bone
	SAFE_DELETE(BoneIndexBuffer);
	// SAFE_RELEASE(ECB_BoneIndexBuffer);
#pragma endregion Bone
}

void SkeletalMesh::Render(UINT InstanceCount) const
{
	if (BoneIndexBuffer != nullptr)
		BoneIndexBuffer->BindToGPU();

	ModelMesh::Render(InstanceCount);
}

void SkeletalMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
	const string CBufferInfo = MeshName + " : Base Bone Index for this Mesh";
	BoneIndexBuffer = new ConstantBuffer(ShaderType::VertexShader,VS_BoneIndex, &BoneIndexData, CBufferInfo, sizeof(BoneIndexDesc), true);
}

int SkeletalMesh::GetBoneIndex() const
{
	return static_cast<int>(BoneIndexData.BaseBoneIndex);
}
void SkeletalMesh::SetBoneIndex(int InBoneIndex)
{
	BoneIndexData.BaseBoneIndex = InBoneIndex;
}

// void SkeletalMesh::SetBoneTransforms( Matrix * Transforms )
// {
// 	this->BoneTransforms = Transforms;
// 	memcpy(BoneData.BoneTransform, Transforms, sizeof(Matrix) * MaxBoneCount);
// }
//
// void SkeletalMesh::SetOffsetMatrix(Matrix * const Transforms)
// {
// 	this->OffsetMatrix = Transforms;
// 	memcpy(BoneData.OffsetMatrix, Transforms, sizeof(Matrix) * MaxBoneCount);
// }

#include "framework.h"
#include "SkeletalMesh.h"

SkeletalMesh::SkeletalMesh()
	: BoneIndexData(), BoneIndexBuffer(nullptr), ECB_BoneIndexBuffer(nullptr)
{
}

SkeletalMesh::~SkeletalMesh()
{
#pragma region Bone
	SAFE_DELETE(BoneIndexBuffer);
	SAFE_RELEASE(ECB_BoneIndexBuffer);
#pragma endregion Bone
}

void SkeletalMesh::Render(UINT InstanceCount) const
{
	BoneIndexBuffer->BindToGPU();
	CHECK(ECB_BoneIndexBuffer->SetConstantBuffer(*BoneIndexBuffer) >= 0);

	ModelMesh::Render(InstanceCount);
}

void SkeletalMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
	
	const string CBufferInfo = MeshName + " : Base Bone Index for this Mesh";
	BoneIndexBuffer = new ConstantBuffer(&BoneIndexData, CBufferInfo, sizeof(BoneIndexDesc));
	ECB_BoneIndexBuffer = CachedShader->AsConstantBuffer(CBufferName);
}

int SkeletalMesh::GetBoneIndex() const
{
	return BoneIndexData.BaseBoneIndex;
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

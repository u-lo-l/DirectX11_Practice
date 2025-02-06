#include "framework.h"
#include "SkeletalMesh.h"

SkeletalMesh::SkeletalMesh()
	: OffsetMatrix(nullptr), BoneTransforms(nullptr), BoneData(), BoneDescBuffer(nullptr), ECB_BoneDescBuffer(nullptr)
{
}

SkeletalMesh::~SkeletalMesh()
{
#pragma region Bone
	SAFE_DELETE(BoneDescBuffer);
	SAFE_RELEASE(ECB_BoneDescBuffer);
#pragma endregion Bone
}

void SkeletalMesh::Tick( UINT InInstanceSize, const vector<ModelAnimation *> & InAnimations )
{
	ModelMesh::Tick(InInstanceSize, InAnimations);
}

void SkeletalMesh::Render(bool bInstancing)
{
	if (CachedShader == nullptr)
		CachedShader = MaterialData->GetShader();
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->BindToGPU(); // FrameRenderer
	
	BoneDescBuffer->BindToGPU();
	CHECK(ECB_BoneDescBuffer->SetConstantBuffer(*BoneDescBuffer) >= 0);
	
	if (ClipsSRVVar != nullptr)
		CHECK(ClipsSRVVar->SetResource(ClipsSRV) >= 0);
	
	if (FrameCBuffer != nullptr)
	{
		FrameCBuffer->BindToGPU();
		CHECK(ECB_FrameBuffer->SetConstantBuffer(*FrameCBuffer) >= 0);
	}
	CachedShader->DrawIndexedInstanced(
		0,
		Pass,
		IndicesCount,
		Model::MaxModelInstanceCount
	);
}

void SkeletalMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
	
	const string CBufferInfo = MeshName + " : Every Bone TF Matrix and Current Bone Index for this Mesh";
	BoneDescBuffer = new ConstantBuffer(&BoneData, CBufferInfo, sizeof(BoneDesc));
	ECB_BoneDescBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_ModelBones");
}

int SkeletalMesh::GetBoneIndex() const
{
	return static_cast<int>(BoneData.BoneIndex);
}
void SkeletalMesh::SetBoneIndex(int InBoneIndex)
{
	BoneData.BoneIndex = InBoneIndex;
}

void SkeletalMesh::SetBoneTransforms( Matrix * Transforms )
{
	this->BoneTransforms = Transforms;
	memcpy(BoneData.BoneTransform, Transforms, sizeof(Matrix) * MaxBoneCount);
}

void SkeletalMesh::SetOffsetMatrix(Matrix * const Transforms)
{
	this->OffsetMatrix = Transforms;
	memcpy(BoneData.OffsetMatrix, Transforms, sizeof(Matrix) * MaxBoneCount);
}

#include "framework.h"
#include "SkeletalMeshSubset.h"

SkeletalMeshSubset::SkeletalMeshSubset(const MeshSubsetDesc& Desc)
	: ARenderable()
{
	ARenderable::SetName(Desc.Name);
	ARenderable::SetMaterial(Desc.Material);
	ARenderable::SetShader();
	Vertices = Desc.Vertices;
	Indices = Desc.Indices;

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
}

SkeletalMeshSubset::~SkeletalMeshSubset()
= default;

void SkeletalMeshSubset::SetSkeletal(const CSkeletal* Skeletal)
{
	this->Skeleton = Skeletal;
	CHECK(!!Skeleton);
}

void SkeletalMeshSubset::BindResources() const
{
	constexpr static int BoneMatrixSRVSlot = 6;
	constexpr static int OffsetMatrixCBSlot = 2;

	CHECK(!!Skeleton);
	CHECK(!!Tf);
	
	BindBuffer();
	// Mat->BindToGpu(1);
	Skeleton->BindToGPU(OffsetMatrixCBSlot, BoneMatrixSRVSlot);
	Tf->BindToGPU(3);
}


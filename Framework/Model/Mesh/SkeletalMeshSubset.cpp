#include "framework.h"
#include "SkeletalMeshSubset.h"

SkeletalMeshSubset::SkeletalMeshSubset(const MeshSubsetDesc& Desc)
	: ARenderable()
{
	ARenderable::SetName(Desc.Name);
	ARenderable::SetMaterial(Desc.Material);
	Shader = Mat->GetRenderingShader();
	VertexInputLayout = Shader->GetInputLayout();
	Vertices = Desc.Vertices;
	Indices = Desc.Indices;
	
	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(SkeletalMeshVertex));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
	InstBuffer = nullptr;
}

SkeletalMeshSubset::~SkeletalMeshSubset()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
}

void SkeletalMeshSubset::SetSkeletal(const CSkeletal* Skeletal)
{
	this->Skeleton = Skeletal;
	CHECK(!!Skeleton);
}

void SkeletalMeshSubset::BindResources() const
{
	// CHECK(!!VertexInputLayout);
	// CHECK(!!InShader);
	CHECK(!!VBuffer);
	CHECK(!!IBuffer);
	CHECK(!!Mat);
	CHECK(!!Skeleton);
	CHECK(!!Tf);
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	Mat->BindToGpu(1);
	Skeleton->BindToGPU(2, 3);
	Tf->BindToGPU(3);
}


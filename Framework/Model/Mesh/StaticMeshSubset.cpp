#include "framework.h"
#include "StaticMeshSubset.h"

StaticMeshSubset::StaticMeshSubset(const MeshSubsetDesc& Desc)
	: ARenderable()
{
	ARenderable::SetName(Desc.Name);
	ARenderable::SetMaterial(Desc.Material);
	Shader = Mat->GetRenderingShader();
	VertexInputLayout = Shader->GetInputLayout();
	Vertices = Desc.Vertices;
	Indices = Desc.Indices;

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexSkeletalMesh));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
	ARenderable::CreateInstanceBuffer(WorldTransforms.data(), WorldTransforms.size(), sizeof(Matrix));
}

StaticMeshSubset::~StaticMeshSubset()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
}

void StaticMeshSubset::BindResources() const
{
	// CHECK(!!VertexInputLayout);
	// CHECK(!!InShader);
	CHECK(!!VBuffer);
	CHECK(!!IBuffer);
	CHECK(!!Mat);
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	InstBuffer->BindToGPU();
	Mat->BindToGpu(1);
}

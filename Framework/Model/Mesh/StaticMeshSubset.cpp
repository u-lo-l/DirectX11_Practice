#include "framework.h"
#include "StaticMeshSubset.h"

StaticMeshSubset::StaticMeshSubset(const MeshSubsetDesc& Desc)
	: ARenderable()
{
	ARenderable::SetName(Desc.Name);
	ARenderable::SetMaterial(Desc.Material);
	ARenderable::SetShader();

	Vertices = Desc.Vertices;
	Indices = Desc.Indices;

	ARenderable::CreateVertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexSkeletalMesh));
	ARenderable::CreateIndexBuffer(Indices.data(), Indices.size());
	ARenderable::CreateInstanceBuffer(WorldTransforms.data(), WorldTransforms.size(), sizeof(Matrix));
}

StaticMeshSubset::~StaticMeshSubset()
= default;

void StaticMeshSubset::BindResources() const
{
	BindBuffer();
	// Mat->BindToGpu(1);
}

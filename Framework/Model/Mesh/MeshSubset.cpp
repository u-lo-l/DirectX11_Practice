#include "framework.h"
#include "MeshSubset.h"

MeshSubset::MeshSubset(const MeshSubsetDesc& Desc)
{
	CHECK(Desc.Name.empty() == false);
	CHECK(Desc.pVertices != nullptr);
	CHECK(Desc.pIndices != nullptr);
	CHECK(Desc.pMaterialData != nullptr);

	this->MeshName = Desc.Name;
	this->Vertices = *Desc.pVertices;
	this->Indices = *Desc.pIndices;
	
	VBuffer = new VertexBuffer(
		this->Vertices.data(),
		this->Vertices.size(),
		sizeof(VertexType)
	);
	IBuffer = new IndexBuffer(
		this->Indices.data(),
		this->Indices.size()
	);
	MaterialData = Desc.pMaterialData;
	Tf = new Transform();
	// Matrix LocalMat = this->Tf->GetMatrix(); 
}

MeshSubset::~MeshSubset()
{
	SAFE_DELETE(VBuffer)
	SAFE_DELETE(IBuffer)
}

void MeshSubset::SetTransform(const Matrix & InMatrix)
{
	this->Tf->SetTransform(InMatrix);
	// Matrix LocalMat = this->Tf->GetMatrix(); 
}

void MeshSubset::Tick()
{
	return ;
}

void MeshSubset::Render() const
{
	MaterialData->BindToGPU(0); // t0, t1, t2

	VBuffer->BindToGPU();
	IBuffer->BindToGPU();

	MaterialData->GetShader()->DrawIndexed(IBuffer->GetCount());
}

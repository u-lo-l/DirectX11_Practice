#include "framework.h"
#include "MeshSubset.h"

MeshSubset::MeshSubset(const MeshSubsetDesc& Desc)
: CB_LocalTransform(nullptr)
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
	Matrix LocalMat = this->Tf->GetMatrix(); 
	CB_LocalTransform = new ConstantBuffer(
		ShaderType::VP,
		2,
		&LocalMat,
		"Local Transform",
		sizeof(Matrix),
		false
	);
}

MeshSubset::~MeshSubset()
{
	SAFE_DELETE(VBuffer)
	SAFE_DELETE(IBuffer)
	SAFE_DELETE(CB_LocalTransform)
}

void MeshSubset::SetTransform(const Matrix & InMatrix)
{
	this->Tf->SetTransform(InMatrix);
	Matrix LocalMat = this->Tf->GetMatrix(); 
	CB_LocalTransform->UpdateData(&LocalMat, sizeof(Matrix));
}

void MeshSubset::Tick()
{
	return ;
}

void MeshSubset::Render() const
{
	CB_LocalTransform->BindToGPU(); // 2
	MaterialData->BindToGPU(0); // t0, t1, t2

	VBuffer->BindToGPU();
	IBuffer->BindToGPU();

	MaterialData->GetShader()->DrawIndexed(IBuffer->GetCount());
}

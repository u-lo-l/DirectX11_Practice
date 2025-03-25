#include "framework.h"
#include "StaticMesh.h"
#include "SkeletalMesh.h"

ModelMesh::ModelMesh()
	: ref_ModelWorldTransform(new Transform())
{
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(WBuffer);
}

// InstanceSize는 Model의 Transforms의 size()
// BlendingDesc의 Current의 Clip정보는 일단 랜덤으로 줬다고 가정하자.
void ModelMesh::Tick()
{
	W_Data.World = Matrix::Identity;
	WBuffer->UpdateData(&W_Data, sizeof(W_Desc));
}

void ModelMesh::Render(UINT InstanceCount) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	WBuffer->BindToGPU();
	
	Context::Get()->GetViewProjectionCBuffer()->BindToGPU();
	
	MaterialData->Render();
	
	// MaterialData->GetShader()->DrawIndexedInstanced(
	// 	Indices.size(),
	// 	InstanceCount
	// );
	MaterialData->GetShader()->DrawIndexed(Indices.size());
}

// 매 Tick마다 Model::Tick에서 호출되어서 Mesh의 WorldTransform을 넣어준다.
void ModelMesh::SetWorldTransform( const Transform * InTransform) const
{
	ref_ModelWorldTransform->SetTRS(InTransform);
}

void ModelMesh::SetMaterialData( Material * InMaterial )
{
	MaterialData = InMaterial;
}

void ModelMesh::ReadMeshFile
(
	const BinaryReader * InReader,
	vector<ModelMesh*> & OutMeshes,
	const map<string, Material*> & InMaterialTable,
	bool bIsSkeletal
)
{
	const UINT MeshCount = InReader->ReadUint();
	OutMeshes.resize( MeshCount );

	for (UINT i = 0; i < MeshCount; i++)
	{
		
		if (bIsSkeletal == false)
			OutMeshes[i] = new StaticMesh();
		else
			OutMeshes[i] = new SkeletalMesh();
		OutMeshes[i]->MeshName = InReader->ReadString();
		
		const string MaterialName = InReader->ReadString();
		if (OutMeshes[i]->MeshName.empty() == true)
		{
			OutMeshes[i]->MeshName = "Mesh #" + to_string(i) + " for " + MaterialName;
		}
		OutMeshes[i]->SetMaterialData(InMaterialTable.at(MaterialName));

		const UINT VertexCount = InReader->ReadUint();
		if (VertexCount > 0)
		{
			OutMeshes[i]->Vertices.resize(VertexCount);
			void * Ptr = OutMeshes[i]->Vertices.data();
			InReader->ReadByte(&Ptr, sizeof(VertexType) * VertexCount);
		}
		
		const UINT IndexCount = InReader->ReadUint();
		if (IndexCount > 0)
		{
			OutMeshes[i]->Indices.resize(IndexCount);
			void * Ptr = OutMeshes[i]->Indices.data();
			InReader->ReadByte(&Ptr, sizeof(UINT) * IndexCount);
		}
	}
}

void ModelMesh::CreateBuffers()
{
	VBuffer = new VertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices.data(), Indices.size());
	WBuffer = new ConstantBuffer(ShaderType::VertexShader, VS_World, &W_Data, "W", sizeof(W_Desc));
}


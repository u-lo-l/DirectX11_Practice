#include "framework.h"
#include "ModelMesh.h"

ModelMesh::ModelMesh()
{
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
}

void ModelMesh::Tick() const
{
	CHECK(Renderer->AsMatrix("World")->SetMatrix(WorldMatrix) >= 0);
	CHECK(Renderer->AsMatrix("View")->SetMatrix(Context::Get()->GetViewMatrix()) >= 0);
	CHECK(Renderer->AsMatrix("Projection")->SetMatrix(Context::Get()->GetProjectionMatrix()) >= 0);
}

void ModelMesh::Render() const
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();

	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer->DrawIndexed(0, 1, IndicesCount);
}

void ModelMesh::ReadFile( const BinaryReader * InReader, vector<ThisClassPtr> & OutMeshes )
{
	UINT MeshCount = InReader->ReadUint();
	OutMeshes.resize( MeshCount );
	for (UINT i = 0; i < MeshCount; i++)
	{
		OutMeshes[i] = new ThisClass();
		OutMeshes[i]->Name = InReader->ReadString();
		OutMeshes[i]->MaterialName = InReader->ReadString();

		OutMeshes[i]->VerticesCount = InReader->ReadUint();
		OutMeshes[i]->Vertices = new VertexType[OutMeshes[i]->VerticesCount];
		if (OutMeshes[i]->VerticesCount > 0)
		{
			void * Ptr = OutMeshes[i]->Vertices;
			InReader->ReadByte(&Ptr, sizeof(VertexType) * OutMeshes[i]->VerticesCount);
		}
		
		OutMeshes[i]->IndicesCount = InReader->ReadUint();
		OutMeshes[i]->Indices = new UINT[OutMeshes[i]->IndicesCount];
		if (OutMeshes[i]->VerticesCount > 0)
		{
			void * Ptr = OutMeshes[i]->Indices;
			InReader->ReadByte(&Ptr, sizeof(UINT) * OutMeshes[i]->IndicesCount);
		}
	}
}

void ModelMesh::BindData(Shader * InRenderer)
{
	Renderer = InRenderer;
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices, IndicesCount);
}

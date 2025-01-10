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
	Shader * const Drawer = MaterialData->GetShader();
	if (CBBinder != nullptr)
		CBBinder->Tick();
	CHECK(Drawer->AsMatrix("World")->SetMatrix(WorldMatrix) >= 0);
	
	// 이 두 줄이 빠지는 이유 : View, Projection은 ConstantBuffer로 넘어갔다. 이제 Context.Tick()에서 관리한다.
	// CHECK(Drawer->AsMatrix("View")->SetMatrix(Context::Get()->GetViewMatrix()) >= 0);
	// CHECK(Drawer->AsMatrix("Projection")->SetMatrix(Context1::Get()->GetProjectionMatrix()) >= 0);
}

void ModelMesh::Render() const
{
	Shader * const Drawer = MaterialData->GetShader();

	CBBinder->BindToGPU();
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Drawer->DrawIndexed(0, 0, IndicesCount);
}

void ModelMesh::ReadFile(
	const BinaryReader * InReader,
	vector<ThisClassPtr> & OutMeshes,
	const map<string, Material*> & InMaterialTable )
{
	UINT MeshCount = InReader->ReadUint();
	OutMeshes.resize( MeshCount );
	for (UINT i = 0; i < MeshCount; i++)
	{
		OutMeshes[i] = new ThisClass();

		OutMeshes[i]->Name = InReader->ReadString();
		
		const string MaterialName = InReader->ReadString();
		OutMeshes[i]->MaterialData = InMaterialTable.at(MaterialName);

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

void ModelMesh::CreateBuffers()
{
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices, IndicesCount);

	CBBinder = new ConstantDataBinder(MaterialData->GetShader());
}

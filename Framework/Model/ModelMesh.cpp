#include "framework.h"
#include "ModelMesh.h"

ModelMesh::ModelMesh()
	: Transforms(nullptr), BoneData(), BoneMatrixCBuffer(nullptr), ECB_BoneMatrixBuffer(nullptr)
{
	WorldTransform = new Transform();
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
}

void ModelMesh::Tick()
{
	if (CBBinder != nullptr)
		CBBinder->Tick();

	BoneData.BoneIndex = BoneIndex;
	memcpy(BoneData.Transforms, Transforms, sizeof(Matrix) * MaxModelTransforms);

	WorldTransform->Tick();
}

void ModelMesh::Render() const
{
	Shader * const Drawer = MaterialData->GetShader();

	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	CBBinder->BindToGPU();
	
	BoneMatrixCBuffer->BindToGPU();
	CHECK(ECB_BoneMatrixBuffer->SetConstantBuffer(*BoneMatrixCBuffer) >= 0);
	
	WorldTransform->Render(Drawer);
	
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Drawer->DrawIndexed(0, Pass, IndicesCount);
}

void ModelMesh::SetWorldTransform( const Transform * InTransform) const
{
	WorldTransform->SetTRS(InTransform);
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

	for (const ThisClassPtr Mesh : OutMeshes)
		Mesh->CreateBuffers();
}

void ModelMesh::CreateBuffers()
{
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices, IndicesCount);
	
	CBBinder = new ConstantDataBinder(MaterialData->GetShader());
	BoneMatrixCBuffer = new ConstantBuffer(&BoneData, "ModelMesh.Bone.Matrix", sizeof(BoneDesc));
	ECB_BoneMatrixBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_ModelBones");
}

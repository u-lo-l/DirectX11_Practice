#include "framework.h"
#include "ModelMesh.h"

ModelMesh::ModelMesh()
	: Transforms(nullptr), BoneData(), BoneMatrixCBuffer(nullptr), ECB_BoneMatrixBuffer(nullptr)
{
	WorldTransform = new Transform();
	bBoneIndexChanged = true;
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

	if (bBoneIndexChanged == true)
	{
		BoneData.BoneIndex = BoneIndex;
		memcpy(BoneData.Transforms, Transforms, sizeof(Matrix) * MaxModelTransforms);
		bBoneIndexChanged = false;
	}
	
	WorldTransform->Tick();
}

void ModelMesh::Render()
{
	if (CachedShader == nullptr)
		CachedShader = MaterialData->GetShader();
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	CBBinder->BindToGPU();
	
	BoneMatrixCBuffer->BindToGPU();
	CHECK(ECB_BoneMatrixBuffer->SetConstantBuffer(*BoneMatrixCBuffer) >= 0);
	
	WorldTransform->BindCBufferToGPU(CachedShader);

	CachedShader->DrawIndexed(0, Pass, IndicesCount);
}

void ModelMesh::SetWorldTransform( const Transform * InTransform) const
{
	WorldTransform->SetTRS(InTransform);
}

void ModelMesh::ReadMeshFile(
	const BinaryReader * InReader,
	vector<ThisClassPtr> & OutMeshes,
	const map<string, Material*> & InMaterialTable )
{
	const UINT MeshCount = InReader->ReadUint();
	OutMeshes.resize( MeshCount );
	for (UINT i = 0; i < MeshCount; i++)
	{
		OutMeshes[i] = new ThisClass();

		OutMeshes[i]->MeshName = InReader->ReadString();
		
		const string MaterialName = InReader->ReadString();
		if (OutMeshes[i]->MeshName.empty() == true)
		{
			OutMeshes[i]->MeshName = "Mesh for " + MaterialName;
		}
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
	
	// 여기서 하나의 모델에 대해 ConstantDataBinder와 ConstantBuffer가 중복으로 생성되는 것 같다.
	// ConstantDataBinder와 : View, Projection, LightDirection들어있음. -> 모델메쉬, 모델과 별개 아닌가?
	// ConstantBuffer = BoneData들어있음 -> 모델마다 하나씩만 있으면 안 되나?
	// TODO : ConstantDataBinder 모델과 독립시키기
	// TODO : BoneDataCBuffer 모델메쉬와 독립시키기
 	CBBinder = new ConstantDataBinder(MaterialData->GetShader());
	const string CBufferInfo = MeshName + "__ModelMesh.Bone.Matrix";
	BoneMatrixCBuffer = new ConstantBuffer(&BoneData, CBufferInfo, sizeof(BoneDesc));
	ECB_BoneMatrixBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_ModelBones");
}

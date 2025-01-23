#include "framework.h"
#include "ModelMesh.h"

#ifdef DO_DEBUG
ModelMesh::ModelMesh( const string & MetaData )
	: Transforms(nullptr), BoneData(), BoneDescBuffer(nullptr), ECB_BoneDescBuffer(nullptr), FrameData(), FrameCBuffer(nullptr),
	ECB_FrameBuffer(nullptr)
{
	this->MetaData = MetaData;
	WorldTransform = new Transform("World Transform of " + MetaData);
	bBoneIndexChanged = true;
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#else
ModelMesh::ModelMesh()
	: Transforms(nullptr), BoneData(), BoneMatrixCBuffer(nullptr), ECB_BoneMatrixBuffer(nullptr)
{
#ifdef DO_DEBUG
	WorldTransform = new Transform("ModelMesh WorldTransform");
#else
	WorldTransform = new Transform();
#endif
	bBoneIndexChanged = true;
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#endif

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
}

void ModelMesh::Tick()
{
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->Tick();
	WorldTransform->Tick();
}

void ModelMesh::Render(int InFrame)
{
	this->FrameData.Frame = InFrame;
	if (CachedShader == nullptr)
		CachedShader = MaterialData->GetShader();

	WorldTransform->BindCBufferToGPU(CachedShader);
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->BindToGPU();
	
	BoneDescBuffer->BindToGPU();
	CHECK(ECB_BoneDescBuffer->SetConstantBuffer(*BoneDescBuffer) >= 0);
	
	FrameCBuffer->BindToGPU();
	CHECK(ECB_FrameBuffer->SetConstantBuffer(*FrameCBuffer) >= 0);
	
	if (ClipsSRVVar != nullptr)
		ClipsSRVVar->SetResource(ClipsSRV);
	

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

#ifdef DO_DEBUG
	printf("Mesh Count : %d\n", MeshCount);
#endif
	for (UINT i = 0; i < MeshCount; i++)
	{
#ifdef DO_DEBUG
		OutMeshes[i] = new ThisClass("Mesh #" + to_string(i));
#else
		OutMeshes[i] = new ThisClass();
#endif
		OutMeshes[i]->MeshName = InReader->ReadString();
		
		const string MaterialName = InReader->ReadString();
		if (OutMeshes[i]->MeshName.empty() == true)
		{
			OutMeshes[i]->MeshName = "Mesh #" + to_string(i) + " for " + MaterialName;
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
	if (GlobalMatrixCBBinder == nullptr)
 		GlobalMatrixCBBinder = new ConstantDataBinder(MaterialData->GetShader());
	
#ifdef DO_DEBUG
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType), MeshName);
#else
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
#endif
	IBuffer = new IndexBuffer(Indices, IndicesCount);

	const string CBufferInfo = MeshName + " : All Model Transform Matrix and Index for this Mesh";
	BoneDescBuffer = new ConstantBuffer(&BoneData, CBufferInfo, sizeof(BoneDesc));
	ECB_BoneDescBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_ModelBones");
	
	FrameCBuffer = new ConstantBuffer(&this->FrameData, "Temp Frame", sizeof(FrameDesc));
	ECB_FrameBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_AnimationFrame");

	ClipsSRVVar = MaterialData->GetShader()->AsSRV("ClipsTFMap");
}


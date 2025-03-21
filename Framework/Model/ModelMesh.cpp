#include "framework.h"
// #include "StaticMesh.h"
// #include "SkeletalMesh.h"
//
// #ifdef DO_DEBUG
// ModelMesh::ModelMesh( const string & MetaData )
// {
// 	this->MetaData = MetaData;
// 	WorldTransform = new Transform("World Transform of " + MetaData);
// 	bBoneIndexChanged = true;
// 	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
// }
// #else
// ModelMesh::ModelMesh()
// 	: ref_ModelWorldTransform(new Transform())//, BlendingDatas{}, FrameCBuffer(nullptr), ECB_FrameBuffer(nullptr)
// {
// 	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
// }
// #endif
//
// ModelMesh::~ModelMesh()
// {
// 	SAFE_DELETE_ARR(Vertices);
// 	SAFE_DELETE_ARR(Indices);
// 	SAFE_DELETE(VBuffer);
// 	SAFE_DELETE(IBuffer);
// 	SAFE_DELETE(GlobalMatrixCBBinder);
// }
//
// // InstanceSize는 Model의 Transforms의 size()
// // BlendingDesc의 Current의 Clip정보는 일단 랜덤으로 줬다고 가정하자.
// void ModelMesh::Tick()
// {
// 	if (GlobalMatrixCBBinder != nullptr)
// 		GlobalMatrixCBBinder->Tick(); // FrameRenderer in Course
// }
//
// void ModelMesh::Render(UINT InstanceCount) const
// {
// 	// if (CachedShader == nullptr)
// 	// 	CachedShader = MaterialData->GetShader();
// 	
// 	VBuffer->BindToGPU();
// 	IBuffer->BindToGPU();
// 	MaterialData->Render();
// 	if (GlobalMatrixCBBinder != nullptr)
// 		GlobalMatrixCBBinder->BindToGPU(); // FrameRenderer
//
// 	CachedShader->DrawIndexedInstanced(
// 		0,
// 		Pass,
// 		IndicesCount,
// 		InstanceCount
// 	);
// }
//
// // 매 Tick마다 Model::Tick에서 호출되어서 Mesh의 WorldTransform을 넣어준다.
// void ModelMesh::SetWorldTransform( const Transform * InTransform) const
// {
// 	ref_ModelWorldTransform->SetTRS(InTransform);
// }
//
// void ModelMesh::SetMaterialData( Material * InMaterial )
// {
// 	MaterialData = InMaterial;
// 	CachedShader = InMaterial->GetShader();
// }
//
// void ModelMesh::ReadMeshFile(
// 	const BinaryReader * InReader,
// 	vector<ThisClassPtr> & OutMeshes,
// 	const map<string, Material*> & InMaterialTable,
// 	bool bIsSkeletal)
// {
// 	const UINT MeshCount = InReader->ReadUint();
// 	OutMeshes.resize( MeshCount );
//
// 	for (UINT i = 0; i < MeshCount; i++)
// 	{
// 		
// 		if (bIsSkeletal == false)
// 			OutMeshes[i] = new StaticMesh();
// 		else
// 			OutMeshes[i] = new SkeletalMesh();
// 		OutMeshes[i]->MeshName = InReader->ReadString();
// 		
// 		const string MaterialName = InReader->ReadString();
// 		if (OutMeshes[i]->MeshName.empty() == true)
// 		{
// 			OutMeshes[i]->MeshName = "Mesh #" + to_string(i) + " for " + MaterialName;
// 		}
// 		OutMeshes[i]->SetMaterialData(InMaterialTable.at(MaterialName));
//
// 		OutMeshes[i]->VerticesCount = InReader->ReadUint();
// 		OutMeshes[i]->Vertices = new VertexType[OutMeshes[i]->VerticesCount];
// 		if (OutMeshes[i]->VerticesCount > 0)
// 		{
// 			void * Ptr = OutMeshes[i]->Vertices;
// 			InReader->ReadByte(&Ptr, sizeof(VertexType) * OutMeshes[i]->VerticesCount);
// 		}
// 		
// 		OutMeshes[i]->IndicesCount = InReader->ReadUint();
// 		OutMeshes[i]->Indices = new UINT[OutMeshes[i]->IndicesCount];
// 		if (OutMeshes[i]->IndicesCount > 0)
// 		{
// 			void * Ptr = OutMeshes[i]->Indices;
// 			InReader->ReadByte(&Ptr, sizeof(UINT) * OutMeshes[i]->IndicesCount);
// 		}
// 	}
// }
//
// void ModelMesh::CreateBuffers()
// {
// 	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
// 	IBuffer = new IndexBuffer(Indices, IndicesCount);
//
// 	// 이거 왜 ModelMesh에 있는지 모르겠음. Material의 Shader마다 있어야하는데
// 	// 일단 귀찮아서 여기다 넣어뒀나봄.
// 	if (GlobalMatrixCBBinder == nullptr) // FrameRender in Course
// 		GlobalMatrixCBBinder = new ConstantDataBinder(MaterialData->GetShader());
// }

#include "framework.h"
#include "StaticMesh.h"
#include "SkeletalMesh.h"

#ifdef DO_DEBUG
ModelMesh::ModelMesh( const string & MetaData )
{
	this->MetaData = MetaData;
	WorldTransform = new Transform("World Transform of " + MetaData);
	bBoneIndexChanged = true;
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#else
ModelMesh::ModelMesh()
	: ref_ModelWorldTransform(new Transform()), BlendingData(), FrameCBuffer(nullptr), ECB_FrameBuffer(nullptr)
{
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#endif

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(GlobalMatrixCBBinder);

#pragma region Animation
	SAFE_DELETE(FrameCBuffer);
	SAFE_RELEASE(ECB_FrameBuffer);
	SAFE_RELEASE(ClipsSRV);
	SAFE_RELEASE(ClipsTexture);
	SAFE_RELEASE(ClipsSRVVar);
#pragma endregion Animation
}

void ModelMesh::Tick(const ModelAnimation * CurrentAnimation)
{
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->Tick();
	
	ref_ModelWorldTransform->Tick();

#pragma region Animation
	if (CurrentAnimation != nullptr)
	{
		UpdateCurrentFrameData(CurrentAnimation);
		if (BlendingData.Next.Clip > -1)
		{
			if (BlendingData.ElapsedBlendTime >= 1.0f)
			{
				BlendingData.Current = BlendingData.Next;
				BlendingData.Next.Clip = -1;
				BlendingData.ElapsedBlendTime = 0.0f;
			}
			else
			{
				UpdateNextFrameData(CurrentAnimation);
				const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
				BlendingData.ElapsedBlendTime += DeltaTime / BlendingData.BlendingDuration;
			}
		}
	}
#pragma endregion Animation
}

void ModelMesh::Render(bool bInstancing)
{
	if (CachedShader == nullptr)
		CachedShader = MaterialData->GetShader();

	ref_ModelWorldTransform->BindCBufferToGPU(CachedShader);
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->BindToGPU();

#pragma region Animation
	if (FrameCBuffer != nullptr)
	{
		FrameCBuffer->BindToGPU();
		CHECK(ECB_FrameBuffer->SetConstantBuffer(*FrameCBuffer) >= 0);
	}
	
	if (ClipsSRVVar != nullptr)
		CHECK(ClipsSRVVar->SetResource(ClipsSRV) >= 0);
#pragma endregion Animation

	if (bInstancing == true)
		CachedShader->DrawIndexedInstanced(0, Pass, IndicesCount, Model::MaxModelInstanceCount);
	else
		CachedShader->DrawIndexed(0, Pass, IndicesCount);
}

// 매 Tick마다 Model::Tick에서 호출되어서 Mesh의 WorldTransform을 넣어준다.
void ModelMesh::SetWorldTransform( const Transform * InTransform) const
{
	ref_ModelWorldTransform->SetTRS(InTransform);
}

void ModelMesh::ReadMeshFile(
	const BinaryReader * InReader,
	vector<ThisClassPtr> & OutMeshes,
	const map<string, Material*> & InMaterialTable,
	bool bIsSkeletal)
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
		if (bIsSkeletal == false)
			OutMeshes[i] = new StaticMesh();
		else
			OutMeshes[i] = new SkeletalMesh();
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
		if (OutMeshes[i]->IndicesCount > 0)
		{
			void * Ptr = OutMeshes[i]->Indices;
			InReader->ReadByte(&Ptr, sizeof(UINT) * OutMeshes[i]->IndicesCount);
		}
	}
}

void ModelMesh::CreateBuffers()
{
	if (GlobalMatrixCBBinder == nullptr)
		GlobalMatrixCBBinder = new ConstantDataBinder(MaterialData->GetShader());
	
#ifdef DO_DEBUG
	printf("---\n");
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType), MeshName);
#else
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
#endif
	
	IBuffer = new IndexBuffer(Indices, IndicesCount);

	if (BlendingData.Current.Clip < 0)
		return ;
	FrameCBuffer = new ConstantBuffer(&this->BlendingData, "Current Animation Blending Description", sizeof(AnimationBlendingDesc));
	ECB_FrameBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_AnimationBlending");
	ClipsSRVVar = MaterialData->GetShader()->AsSRV("ClipsTFMap");
}

void ModelMesh::UpdateCurrentFrameData(const ModelAnimation * InAnimation)
{
	UpdateFrameData(InAnimation, BlendingData.Current);	
}

void ModelMesh::UpdateNextFrameData(const ModelAnimation * InAnimation)
{
	UpdateFrameData(InAnimation, BlendingData.Next);	
}

void ModelMesh::UpdateFrameData(const ModelAnimation * InAnimation, FrameDesc & Frame )
{
	const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
	Frame.CurrentTime = InAnimation->CalculateNextTime(Frame.CurrentTime, DeltaTime);
	const pair<int, int> CurrAndNextFrame = InAnimation->GetCurrentAndNextFrame(Frame.CurrentTime);
	Frame.CurrentFrame = CurrAndNextFrame.first;
	Frame.NextFrame = CurrAndNextFrame.second;
}

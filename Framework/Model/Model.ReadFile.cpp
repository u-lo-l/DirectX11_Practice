#include "framework.h"
#include "Model.h"
#include <fstream>
#include <iomanip>

void Model::ReadFile( const wstring & InFileFullPath )
{
	ifstream ifs;
	ifs.open(InFileFullPath);

	Json::Value Root;
	ifs >> Root;
	ifs.close();

	Json::Value::Members Members = Root.getMemberNames();
	
	Json::Value material	= Root["File"]["Material"];
	Json::Value Mesh		= Root["File"]["Mesh"];
	Json::Value Position	= Root["Transform"]["Position"];
	Json::Value Rotation	= Root["Transform"]["Rotation"];
	Json::Value Scale		= Root["Transform"]["Scale"];

	wstring MaterialName = String::ToWString(material.asString());
	wstring MeshName = String::ToWString(Mesh.asString());
	
	ReadMaterial(MaterialName);
	// ReadMesh안에서 Skeleton::SetupBoneTable()호출
	ReadMeshAndCreateBoneTable(MeshName + L"/" + MeshName);

	// World Transform
	ASSERT(WorldTransform != nullptr, "WorldTransform Not Valid")
	vector<string> pString;
	String::SplitString(&pString, Position.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Rotation.asString(), ",");
	WorldTransform->SetRotation({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Scale.asString(), ",");
	WorldTransform->SetScale({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	// Animation
	Json::Value Animations = Root["Animations"];
	const UINT AnimationCount = Animations.size();
	this->Animations.reserve(AnimationCount);
	for (UINT i = 0; i < AnimationCount; i++)
	{
		ReadAnimation(String::ToWString(Animations[i].asString()));
	}

	if (Animations.empty() == false)
	{
		CreateAnimationTexture(); // this->ClipSRV에 AnimationTexture 저장.
		// for (const auto & Pair : MaterialsTable)
		// {
			// ClipSRVVariables.emplace_back(Pair.second->GetShader()->AsSRV("ClipsTFMap"));
		// }
	}
	
	if (SkeletonData != nullptr)
	{
		SkeletonData->CreateBuffer();
		SkeletonData->ClearBoneTable();
	}

	CreateAnimationBuffers();
	for (ModelMesh * M : this->Meshes)
	{
		M->CreateBuffers();
	}
}

void Model::ReadMaterial( const wstring & InFileName)
{
	const wstring FullPath = W_MATERIAL_PATH + InFileName + L".material";
	
	ifstream Stream;
	Stream.open(FullPath);

	Json::Value Root;
	Stream >> Root;

	const Json::Value::Members Members = Root.getMemberNames();
#ifdef DO_DEBUG
	printf("Model : %s -> Material Count : %d\n", ModelName.c_str(), Members.size());
#endif
	for (const Json::String & Name : Members)
	{
#ifdef DO_DEBUG
		Material * MatData = new Material("Material : [" + String::ToString(InFileName) + "_" + Name + "]");
#else
		Material * MatData = new Material();
#endif
		Json::Value Value = Root[Name];

		ReadShaderName(Value, MatData);
		ReadColor(Value, MatData);
		ReadColorMap(Value, MatData);

		MaterialsTable[Name] = MatData;
	}

	Stream.close();
}

void Model::ReadShaderName(const Json::Value & Value, Material * MatData)
{
	if (Value["ShaderName"].asString().size() > 0)
		MatData->SetShader(String::ToWString(Value["ShaderName"].asString()));
}

void Model::ReadColor(const Json::Value & Value, Material * MatData)
{
	MatData->SetAmbient(JsonStringToColor(Value["Ambient"].asString()));
	MatData->SetDiffuse(JsonStringToColor(Value["Diffuse"].asString()));
	MatData->SetSpecular(JsonStringToColor(Value["Specular"].asString()));
	MatData->SetEmissive(JsonStringToColor(Value["Emissive"].asString()));
}

void Model::ReadColorMap(const Json::Value & Value, Material * MatData)
{
	UINT count = Value["DiffuseMap"].size();
	for (UINT i = 0; i < count; i++)
	{
		if (Value["DiffuseMap"][i].asString().size() > 0)
			MatData->SetDiffuseMap(String::ToWString(Value["DiffuseMap"][i].asString()));
	}

	count = Value["SpecularMap"].size();
	for (UINT i = 0; i < count; i++)
	{
		if (Value["SpecularMap"][i].asString().size() > 0)
			MatData->SetSpecularMap(String::ToWString(Value["SpecularMap"][i].asString()));
	}

	count = Value["NormalMap"].size();
	for (UINT i = 0; i < count; i++)
	{
		if (Value["NormalMap"][i].asString().size() > 0)
			MatData->SetNormalMap(String::ToWString(Value["NormalMap"][i].asString()));
	}
}

void Model::ReadMeshAndCreateBoneTable( const wstring & InFileName  )
{
	const wstring FullPath = W_MODEL_PATH + InFileName + L".mesh";

	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);

	ModelBone::ReadModelFile(BinReader, this->SkeletonData);
	bool bIsSkeletal = this->SkeletonData != nullptr;
	ModelMesh::ReadMeshFile(BinReader, this->Meshes, this->MaterialsTable, bIsSkeletal);
	
	BinReader->Close();
	SAFE_DELETE(BinReader);

	if (bIsSkeletal == true)
		this->SkeletonData->SetUpBoneTable(this->Meshes);
}

void Model::ReadAnimation( const wstring & InFileName )
{
	BinaryReader * BinReader = new BinaryReader(W_MODEL_PATH + InFileName + L".animation");

	const CachedBoneTableType * BoneTable = nullptr;
	if (this->SkeletonData != nullptr)
		BoneTable = this->SkeletonData->GetCachedBoneTable();
	ModelAnimation * Anim = ModelAnimation::ReadAnimationFile(BinReader, BoneTable); 
	this->Animations.emplace_back(Anim);
	
	SAFE_DELETE(BinReader);
	if (Animations.size() > 0)
	{
		SetClipIndex(0, 0);
	}
}

void Model::CreateAnimationTexture()
{
	if (SkeletonData == nullptr)
		return ;
	
	const UINT AnimationCount = Animations.size();
	vector<ModelAnimation::KeyFrameTFTable *> ClipTFTables;
	ClipTFTables.reserve(AnimationCount);
	// 각 애니메이션의 Transform정보를 Table에 저장.
	for (UINT i = 0; i < AnimationCount; i++)
	{
		ClipTFTables.push_back (Animations[i]->CalcClipTransform(this->SkeletonData));		
	}
	
	{
		constexpr UINT PixelChannel = 4; // 그래서 Format == DXGI_FORMAT_R32G32B32A32_FLOAT이다.
		D3D11_TEXTURE2D_DESC TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(TextureDesc));
		TextureDesc.Width = SkeletalMesh::MaxBoneCount * PixelChannel;
		TextureDesc.Height = ModelAnimation::MaxFrameLength;
		// 전체 애니메이션 개수만큼 Texture가 몇 장 만들어질 지 결정된다.
		TextureDesc.ArraySize = Animations.size();
		TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16Byte * 4 = 64 Byte
		TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.MipLevels = 1;
		TextureDesc.SampleDesc.Count = 1;

		// 가상 메모리 예약
		constexpr UINT PageSize = ModelAnimation::MaxFrameLength * SkeletalMesh::MaxBoneCount * sizeof(Matrix);
		void * ReservedVirtualMemory  = VirtualAlloc(nullptr, PageSize * AnimationCount, MEM_RESERVE, PAGE_READWRITE);

#ifdef DO_DEBUG
		// 가상 메모리 디버깅 정보 출력
		MEMORY_BASIC_INFORMATION MemInfo = {};
		const SIZE_T BytesWritten = VirtualQuery(ReservedVirtualMemory , &MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
		printf("\n=========================================================================\n");
		printf("\t[VMEM DEBUG] %s Bytes Written: %d\n", __FUNCTION__, BytesWritten);
		printf("\t[VMEM DEBUG] %s Memory Base Address: %p\n", __FUNCTION__, MemInfo.BaseAddress);
		printf("\t[VMEM DEBUG] %s Allocation Base: %p\n", __FUNCTION__, MemInfo.AllocationBase);
		printf("\t[VMEM DEBUG] %s Page Size : %zu_bytes\n", __FUNCTION__, PageSize);
		printf("\t[VMEM DEBUG] %s Region Size: %zu_bytes | %zu_KB | %zu_MB\n", __FUNCTION__, MemInfo.RegionSize, MemInfo.RegionSize / 1024, MemInfo.RegionSize / 1024 / 1024);
		printf("\t[VMEM DEBUG] %s State: %s\n", __FUNCTION__, (MemInfo.State == MEM_COMMIT) ? "COMMIT" : (MemInfo.State == MEM_RESERVE) ? "RESERVE" : "FREE");
		printf("\t[VMEM DEBUG] %s Protection: %u\n", __FUNCTION__, MemInfo.Protect);
		printf("=========================================================================\n\n");
#endif
		// 애니메이션 데이터를 가상메모리에 저장.
		for (UINT PageIndex = 0; PageIndex < AnimationCount ; PageIndex++)
		{
			BYTE * AnimationPageAddress = static_cast<BYTE *>(ReservedVirtualMemory) + PageIndex * PageSize; // Animation 지금은 하나라 PageIndex우선 0이다.
			for (UINT FrameIndex = 0; FrameIndex < ModelAnimation::MaxFrameLength ; FrameIndex++)
			{
				void * CurrentFrameAddressInPage = AnimationPageAddress + sizeof(Matrix) * SkeletalMesh::MaxBoneCount * FrameIndex;
				// 페이지 메모리 할당 및 데이터 복사
				VirtualAlloc(CurrentFrameAddressInPage, SkeletalMesh::MaxBoneCount * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(CurrentFrameAddressInPage, ClipTFTables[PageIndex]->TransformMats[FrameIndex], SkeletalMesh::MaxBoneCount * sizeof(Matrix));
			}
		}

#pragma region SSD to VRAM
		// GPU에 텍스처 업로드
		D3D11_SUBRESOURCE_DATA * SubResources = new D3D11_SUBRESOURCE_DATA[AnimationCount];
		for (UINT AnimationIndex = 0; AnimationIndex < AnimationCount ; AnimationIndex++)
		{
			void * AnimationPageAddress = static_cast<BYTE *>(ReservedVirtualMemory) + AnimationIndex * PageSize;
			SubResources[AnimationIndex].pSysMem = AnimationPageAddress;
			SubResources[AnimationIndex].SysMemPitch = SkeletalMesh::MaxBoneCount * sizeof(Matrix); // 가로 길이
			SubResources[AnimationIndex].SysMemSlicePitch = PageSize;						// 전체 배열 크기
		}

		ID3D11Texture2D * ClipTexture; // ShaderResourceView만들기 위한 임시 텍스쳐 생성
		CHECK(D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, SubResources, &ClipTexture) >= 0);
		SAFE_DELETE_ARR(SubResources);
#pragma endregion
		
#pragma region Clear Memory
		for (ModelAnimation::KeyFrameTFTable * TFTable : ClipTFTables)
		{
			SAFE_DELETE(TFTable);			
		}
		VirtualFree(ReservedVirtualMemory , 0, MEM_RELEASE);
#pragma endregion
		
#pragma region Create SRV // 셰이더 리소스 뷰 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.MipLevels = 1;
		SRVDesc.Texture2DArray.MostDetailedMip = 0;
		SRVDesc.Texture2DArray.ArraySize = AnimationCount;

		CHECK(D3D::Get()->GetDevice()->CreateShaderResourceView(ClipTexture, &SRVDesc, &this->ClipSRV2DArray) >= 0);
		SAFE_RELEASE(ClipTexture);
#pragma endregion
	}
}

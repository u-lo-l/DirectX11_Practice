#include "framework.h"
#include "Model.h"
#include <fstream>

void Model::ReadFile( const wstring & InFileFullPath )
{
	ifstream ifs;
	ifs.open(InFileFullPath);

	Json::Value Root;
	ifs >> Root;
	ifs.close();

	Json::Value::Members Members = Root.getMemberNames();
	
	Json::Value material = Root["File"]["Material"];
	Json::Value Mesh = Root["File"]["Mesh"];
	Json::Value Position = Root["Transform"]["Position"];
	Json::Value Rotation = Root["Transform"]["Rotation"];
	Json::Value Scale = Root["Transform"]["Scale"];
	

	wstring MaterialName = String::ToWString(material.asString());
	wstring MeshName = String::ToWString(Mesh.asString());
	
	ReadMaterial(MaterialName);
	ReadMesh(MeshName + L"/" + MeshName);

	if (WorldTransform == nullptr)
		WorldTransform = new Transform();

	vector<string> pString;
	String::SplitString(&pString, Position.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Rotation.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Scale.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});
	
	Json::Value Animations = Root["Animations"];
	const UINT AnimationCount = Animations.size();
	for (UINT i = 0; i < AnimationCount; i++)
	{
		ReadAnimation(String::ToWString(Animations[i].asString()));
	}

	if (Animations.empty() == false)
	{
		CreateAnimationTexture();
		for (ModelMesh * M : this->Meshes)
		{
			M->ClipsTexture = ClipTexture;
			M->ClipsSRV = ClipSRV;
		}
	}
	SAFE_DELETE(CachedBoneTable);
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
		Material * MatData = new Material();
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

void Model::ReadMesh( const wstring & InFileName)
{
	const wstring FullPath = W_MODEL_PATH + InFileName + L".mesh";

	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);

	ModelBone::ReadModelFile(BinReader, this->Bones);
	ModelMesh::ReadMeshFile(BinReader, this->Meshes, this->MaterialsTable);
#ifdef DO_DEBUG
	printf("Model : %s -> Bone Count : %d\n", ModelName.c_str(), Bones.size());
	printf("Model : %s -> Mesh Count : %d\n", ModelName.c_str(), Meshes.size());
#endif
	BinReader->Close();
	SAFE_DELETE(BinReader);

	const UINT BoneCount = this->Bones.size();
	if (BoneCount == 0)
		return ;
	this->CachedBoneTable = new CachedBoneTableType(); // 애니메이션 다 읽으면 지워진다.
	for (UINT i = 0; i < BoneCount; i++)
	{
		ModelBone * TargetBone = this->Bones[i];
		this->BoneTransforms[i] = TargetBone->Transform;
		for (UINT number : TargetBone->MeshIndices)
		{
			Meshes[number]->SetBoneIndex(TargetBone->Index);
			Meshes[number]->Bone = TargetBone;
			Meshes[number]->Transforms = BoneTransforms;
		}
		(*CachedBoneTable)[TargetBone->Name] = TargetBone;
	}
}

void Model::ReadAnimation( const wstring & InFileName )
{
	BinaryReader * BinReader = new BinaryReader(W_MODEL_PATH + InFileName + L".animation");

	ModelAnimation * Anim = ModelAnimation::ReadAnimationFile(BinReader, this->CachedBoneTable); 
	Animations.push_back(Anim);
	
	SAFE_DELETE(BinReader);
}

// TODO : 미완
void Model::CreateAnimationTexture()
{
	const UINT AnimationCount = Animations.size();
	vector<ModelAnimation::KeyFrameTFTable *> ClipTFTables;
	ClipTFTables.reserve(AnimationCount);
	
	for (UINT i = 0; i < AnimationCount; i++)
	{
		ClipTFTables.push_back (Animations[i]->CalcClipTransform(Bones));		
	}

	{
		D3D11_TEXTURE2D_DESC TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(TextureDesc));
		TextureDesc.Width = Model::MaxModelTransforms * 4;
		TextureDesc.Height = ModelAnimation::MaxFrameLength;
		TextureDesc.ArraySize = Animations.size();
		TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16Byte * 4 = 64 Byte
		TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.MipLevels = 1;
		TextureDesc.SampleDesc.Count = 1;

		// 가상 메모리 사용
		const UINT PageSize = TextureDesc.Width * TextureDesc.Height * 16;
		void * p = VirtualAlloc(nullptr, PageSize * AnimationCount, MEM_RESERVE, PAGE_READWRITE);

#ifdef DO_DEBUG
		MEMORY_BASIC_INFORMATION MemInfo = {};
		const SIZE_T BytesWritten = VirtualQuery(p, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION));
		printf("%u\n", BytesWritten);
#endif

		for (UINT c = 0; c < AnimationCount ; c++)
		{
			const UINT start = c * PageSize;
			for (UINT f = 0; f < ModelAnimation::MaxFrameLength ; f++)
			{
				// TODO : why?
				// if (f > Animations[c]->Duration)
				// 	break;
				//
				void * temp = (BYTE *)p + sizeof(Matrix) * Model::MaxModelTransforms * f + start;
				VirtualAlloc(temp, Model::MaxModelTransforms * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, ClipTFTables[c]->TransformMats[f], Model::MaxModelTransforms * sizeof(Matrix));
			}
		}

#pragma region SSD to VRAM

		D3D11_SUBRESOURCE_DATA * SubResource = new D3D11_SUBRESOURCE_DATA[AnimationCount];
		for (UINT c = 0; c < AnimationCount ; c++)
		{
			void * temp = (BYTE *)p + c * PageSize;
			SubResource[c].pSysMem = temp;
			SubResource[c].SysMemPitch = Model::MaxModelTransforms * sizeof(Matrix);
			SubResource[c].SysMemSlicePitch = PageSize;
		}
		D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, SubResource, &ClipTexture);
		SAFE_DELETE_ARR(SubResource);

#pragma endregion
#pragma region Clear Memory
	
		for (ModelAnimation::KeyFrameTFTable * TFTable : ClipTFTables)
		{
			SAFE_DELETE(TFTable);			
		}
		VirtualFree(p, 0, MEM_RELEASE);
		
#pragma endregion
#pragma region Create SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.MipLevels = 1;
		SRVDesc.Texture2DArray.ArraySize = AnimationCount;

		CHECK(D3D::Get()->GetDevice()->CreateShaderResourceView(ClipTexture, &SRVDesc, &this->ClipSRV) >= 0);
#pragma endregion
	}
}

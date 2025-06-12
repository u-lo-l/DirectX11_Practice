#include "Pch.h"
#include "MeshConverter.h"
#include "AssimpLoader/Types.h"

MeshConverter::MeshConverter() : Converter2() { }

MeshConverter::~MeshConverter() = default;

void MeshConverter::ReadAiScene(const wstring& InFileName)
{
	const string Path = ASSET_PATH + String::ToString(InFileName);
	const aiScene * Scene = Importer->ReadFile(
		Path.c_str(),
		ConvertFlag
	);
	ASSERT(!!Scene, Importer->GetErrorString());

	ExportMaterial(InFileName, Scene);
	ExportMesh(InFileName, Scene);
	ExportAsset(Path::GetFileNameWithoutExtension(InFileName));
	
	Importer->FreeScene();
}

void MeshConverter::ExportMaterial(const wstring & InFileName, const aiScene * Scene)
{
	vector<MaterialData*> Materials;

	const string ModelPath = (MODEL_PATH + String::ToString(InFileName));
	const wstring MaterialDirectory = Path::GetDirectoryName(W_MATERIAL_PATH + InFileName); 
	const wstring TextureDirectory = Path::GetDirectoryName(String::ToWString(ModelPath) + L"Textures/");
	
	ReadMaterial(Scene, Materials);
	WriteMaterial(Scene, InFileName, Materials);
	
	for (MaterialData * Material : Materials)
		SAFE_DELETE(Material);
}

void MeshConverter::ExportMesh(const wstring & InFileName, const aiScene* InScene)
{
	const wstring ModelDirectory = Path::GetDirectoryName(W_MODEL_PATH + InFileName);
	const wstring ModelName = Path::GetFileNameWithoutExtension(InFileName);
	const wstring ModelPath = ModelDirectory + ModelName + L".model";
	
	const wstring MeshDirectory = ModelDirectory + L"Mesh/";
	const wstring MeshPath = MeshDirectory + ModelName + L".mesh";

	vector<BoneData*> Bones;
	vector<MeshData*> Meshes;

	ReadBoneRecursive(InScene->mRootNode, 0, -1, Bones);
	ReadMesh(InScene, Meshes);
	ReadSkinningWeight(InScene, Bones, Meshes);

	WriteSkin(MeshPath, Bones, Meshes);

	for (const BoneData * Bone : Bones)
		SAFE_DELETE(Bone);
	for (const MeshData * Mesh : Meshes)
		SAFE_DELETE(Mesh);
}

void MeshConverter::ExportAsset(const wstring & InFileName)
{
	Json::Value Root;

	Json::Value File;
	File["Materials"] = String::ToString(InFileName);
	File["Mesh"] = String::ToString(InFileName);
	Root["File"] = File;

	Json::Value Transform;
	Transform["Position"] = "0.0,0.0,0.0";
	Transform["Rotation"] = "0.0,0.0,0.0";
	Transform["Scale"]	  = "0.1,0.1,0.1";
	Root["Transform"]     = Transform;

	Json::StyledWriter Writer;
	string JsonString = Writer.write(Root);

	ASSERT(JsonString.empty() == false, String::Format("%s : JsonString Empty", __FUNCTION__).c_str());
	
	ofstream OutFileStream;
	wstring OutFilePath = W_MODEL_PATH + InFileName + L".model";
	OutFileStream.open(OutFilePath.c_str());
	OutFileStream << JsonString;
	OutFileStream.close();
}

void MeshConverter::ReadMaterial(const aiScene * Scene, vector<MaterialData*> & OutMaterialData)
{
	OutMaterialData.resize(Scene->mNumMaterials, nullptr);
	
	for (UINT i = 0; i < Scene->mNumMaterials; i++)
	{
		const aiMaterial* Material = Scene->mMaterials[i];
		OutMaterialData[i] = new MaterialData();

		OutMaterialData[i]->Name = Material->GetName().C_Str();
		OutMaterialData[i]->ShaderName = "Mesh/Mesh.hlsl";
		
		aiColor4D color;
		Material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		OutMaterialData[i]->Ambient = Color(color.r, color.g, color.b, color.a);
		
		Material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		OutMaterialData[i]->Diffuse = Color(color.r, color.g, color.b, color.a);
		
		float shininess = 0.0f;
		Material->Get(AI_MATKEY_SHININESS, shininess);
		Material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		OutMaterialData[i]->Specular = Color(color.r, color.g, color.b, shininess);
		
		Material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		OutMaterialData[i]->Emissive = Color(color.r, color.g, color.b, color.a);

		OutMaterialData[i]->CollectTexturePaths(Material, aiTextureType_DIFFUSE);
		OutMaterialData[i]->CollectTexturePaths(Material, aiTextureType_SPECULAR);
		OutMaterialData[i]->CollectTexturePaths(Material, aiTextureType_NORMALS);
	}
}

void MeshConverter::WriteMaterial(
	const aiScene * InScene,
	const wstring & InFileName,
	const vector<MaterialData*> & InMaterials
)
{
	const string ModelDirectory = Path::GetDirectoryName(MODEL_PATH + String::ToString(InFileName));
	const wstring MaterialDirectory = Path::GetDirectoryName(W_MATERIAL_PATH + InFileName);
	const string TextureDirectory = ModelDirectory + "Textures/";
	const wstring FileNameNoExt = Path::GetFileNameWithoutExtension(InFileName);

	Path::CreateFolder(ModelDirectory);
	Path::CreateFolder(MaterialDirectory);
	Path::CreateFolder(TextureDirectory);

	Json::Value Root;
	for (const MaterialData * Data : InMaterials)
	{
		Json::Value Value;
		Value["ShaderName"] = Data->ShaderName;

		Value["Ambient"] = Helper::ColorToJson(Data->Ambient);
		Value["Diffuse"] = Helper::ColorToJson(Data->Diffuse);
		Value["Specular"] = Helper::ColorToJson(Data->Specular);
		Value["Emissive"] = Helper::ColorToJson(Data->Emissive);

		for (const string & Name : Data->DiffuseFiles)
		{
			const string & TexturePath = SaveTextureAsFile(InScene, TextureDirectory, Path::GetFileName(Name));
			if (TexturePath.empty()) continue;
			Value["DiffuseMap"].append(TexturePath);
		}
		for (const string & Name : Data->SpecularFiles)
		{
			const string & TexturePath = SaveTextureAsFile(InScene, TextureDirectory, Path::GetFileName(Name));
			if (TexturePath.empty()) continue;
			Value["SpecularMap"].append(TexturePath);
		}
		for (const string & Name : Data->NormalFiles)
		{
			const string & TexturePath = SaveTextureAsFile(InScene, TextureDirectory, Path::GetFileName(Name));
			if (TexturePath.empty()) continue;
			Value["NormalMap"].append(TexturePath);
		}
		Root[Data->Name.c_str()] = Value;
	}

	wstring SaveMaterialFileName = MaterialDirectory + FileNameNoExt + L".material";

	Json::StyledWriter JsonWriter;
	string JsonString = JsonWriter.write(Root);

	ofstream OutputFileStream;
	OutputFileStream.open(SaveMaterialFileName);
	OutputFileStream << JsonString;
	OutputFileStream.close();
}

string MeshConverter::SaveTextureAsFile(const aiScene * InScene, const string & InSaveFolder, const string & InFilePath)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const Context = D3D::Get()->GetDeviceContext();
	
	ASSERT(!!InScene, "Assimp Importer Scene is not valid")
	ASSERT(InSaveFolder.empty() == false, "InSaveFolder is not valid")
	ASSERT(InFilePath.empty() == false, "InFileName is not valid")

	const string FileName = Path::GetFileName(InFilePath);
	const aiTexture * Texture = InScene->GetEmbeddedTexture(FileName.c_str());
	if (Texture != nullptr)
	{
		if (Texture->mHeight == 0) // jpeg나 png처럼 압축된 형식의 image
		{
			// const BinaryWriter * BinWriter = new BinaryWriter(String::ToWString(InSaveFolder + filename));
			BinaryWriter * const BinWriter = new BinaryWriter(InSaveFolder + FileName);
			BinWriter->WriteByte(Texture->pcData, Texture->mWidth);
			delete BinWriter;
			
			return InSaveFolder + FileName;
		}
		// Texture정보가 decoding된 경우
		// 1. AiTexture를 바탕으로 ID3D11Texture2D를 우선 만들어야해
		D3D11_TEXTURE2D_DESC TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(TextureDesc));
		TextureDesc.Width = Texture->mWidth;
		TextureDesc.Height = Texture->mHeight;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.MipLevels = 0;
		TextureDesc.ArraySize = 1; // LOD(레벨 오브 디테일)용 배열 크기. 단일 텍스처만 사용.
		TextureDesc.SampleDesc.Count = 1; // MSAA 비활성화 (샘플링 없음)
		TextureDesc.SampleDesc.Quality = 0; // MSAA 품질 설정 비활성화
		TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		
		D3D11_SUBRESOURCE_DATA SubResourceData;
		ZeroMemory(&SubResourceData, sizeof(SubResourceData));
		SubResourceData.pSysMem = Texture->pcData;
		SubResourceData.SysMemPitch = Texture->mWidth * 4;

		ID3D11Texture2D * SavingTexture;
		HRESULT Hr = Device->CreateTexture2D(&TextureDesc, &SubResourceData, &SavingTexture);
		
		// 2. 그 다음 ID3DTexture2D를 DirectX::Image로 캡쳐
		DirectX::ScratchImage ScratchImage;
		CHECK(DirectX::CaptureTexture(Device, Context, SavingTexture, ScratchImage) >= 0);
		const DirectX::Image * SavingImage = ScratchImage.GetImage(0, 0, 0);

		// 3. Image를 이용해서 png파일 작성
		Hr = DirectX::SaveToWICFile(
				*SavingImage,
				DirectX::WIC_FLAGS_NONE,
				DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
				String::ToWString(InSaveFolder + FileName).c_str()
			);
		CHECK(Hr < 0);
		return InSaveFolder + FileName;
	}
	else // FBX 내 임베디드 텍스처가 없는 경우, FBX 파일과 동일한 디렉토리에서 텍스처 파일을 찾음.
	{
		const string Directory = Path::GetDirectoryName(InFilePath);
		string OriginPath = InFilePath;
		String::Replace(&OriginPath, "\\", "/");

		if (Path::IsFileExist(OriginPath) == false)
		{
			ASSERT(false, String::Format("%s Not Found", OriginPath).c_str())
			return "";
		}
		
		const string Path = InSaveFolder + "/" + FileName;
		CopyFileA(OriginPath.c_str(), Path.c_str(), false);
		return InSaveFolder + Path::GetFileName(Path);
	}
}

void MeshConverter::ReadBoneRecursive(const aiNode* InNode, int InIndex, int InParentIndex, vector<BoneData *> & OutBones)
{
	BoneData * Bone = new BoneData();
	Bone->Index = InIndex;
	Bone->Parent = InParentIndex;
	Bone->Name = InNode->mName.C_Str();
	Bone->Transform = InNode->mTransformation;
	Bone->Transform.Transpose(); // row-major(Assimp) to colum-major(DX)
	if (Bone->IsRootBone() == false)
	{
		Bone->Transform = Bone->Transform * OutBones[Bone->Parent]->Transform;
	}
	else
	{
		Bone->Transform = Bone->Transform * Matrix::CreateFromEulerAngleInRadian({0, PRE_Y_ROTATION, 0});
	}
	OutBones.push_back(Bone);

	const UINT MeshCount = InNode->mNumMeshes;
	Bone->MeshIndices.reserve(MeshCount);
	for (UINT i	= 0 ; i < MeshCount ; i++)
	{
		// 이 Bone이 몇 번 째(Index의) Mesh에 영향을 주는가에 대한 정보. 
		Bone->MeshIndices.push_back(InNode->mMeshes[i]);
	}

	const UINT ChildBoneCount = InNode->mNumChildren;
	for (UINT i = 0; i < ChildBoneCount ; i++)
	{
		ReadBoneRecursive(InNode->mChildren[i], OutBones.size(), InIndex, OutBones);
	}
}

void MeshConverter::ReadMesh(const aiScene* InScene, vector<MeshData *> & OutMeshes)
{
	const UINT MeshCount = InScene->mNumMeshes;
	OutMeshes.resize(MeshCount);
	for (UINT i = 0; i < MeshCount; i++)
	{
		OutMeshes[i] = new MeshData();
		const aiMesh * const AiMesh = InScene->mMeshes[i];
		const aiNode * MeshNode = InScene->mRootNode->FindNode(AiMesh->mName);
		aiMatrix4x4 MeshTransform = aiMatrix4x4();

		// MeshNode의 ParentNode가 Bone임을 가정. 아니면 UB
		// 실제론 ParentNode가 Bone임을 보장하지 않음.
		if (MeshNode != nullptr)
			MeshTransform = MeshNode->mTransformation;
			
		// Read Material Data
		const UINT MatIndex = AiMesh->mMaterialIndex;
		OutMeshes[i]->MaterialName = InScene->mMaterials[MatIndex]->GetName().C_Str();

		// Read Vertices Data
		const UINT VerticesCount = AiMesh->mNumVertices;
		OutMeshes[i]->Vertices.reserve(VerticesCount);
		for (UINT v = 0; v < VerticesCount; v++)
		{
			OutMeshes[i]->Vertices.emplace_back(ReadSingleVertex(AiMesh, v, MeshTransform));
		}

		// Read Indices Data
		const UINT FacesCount = AiMesh->mNumFaces;
		for (UINT f = 0; f < FacesCount; f++)
		{
			const aiFace & Face = AiMesh->mFaces[f];
			const UINT IndicesCount = Face.mNumIndices;
			for (UINT k = 0; k < IndicesCount; k++)
			{
				OutMeshes[i]->Indices.push_back(Face.mIndices[k]);
			}
		}
	}
}

void MeshConverter::ReadSkinningWeight(
	const aiScene* InScene,
	const vector<BoneData *> & InBones,
	vector<MeshData *> & InOutMeshes
)
{
	map<string, BoneData *> BoneSearchMap;
	for (BoneData * const Bone : InBones)
	{
		BoneSearchMap.insert(make_pair(Bone->Name, Bone));
	}
		
	const UINT MeshCount = InScene->mNumMeshes;
	for (UINT MeshIndex = 0; MeshIndex < MeshCount; MeshIndex++)
	{
		const aiMesh * const TargetMesh = InScene->mMeshes[MeshIndex];
		// Bone이아니다 -> 스키닝이 될 Mesh가 아니다.
		if (TargetMesh->HasBones() == false) 
		{
			continue;
		}
		const UINT BoneCount = TargetMesh->mNumBones;
		for (UINT boneIndex = 0; boneIndex < BoneCount; boneIndex++)
		{
			const aiBone * Bone = TargetMesh->mBones[boneIndex];
			const char * BoneName = Bone->mName.C_Str();
			const auto It = BoneSearchMap.find(BoneName);
			if (It == BoneSearchMap.cend())
				break; // Bone NotFound
			const UINT TargetBoneIndex = It->second->Index;

			// aiBone은 어떤 Vertex에 대해 얼마나 weight를 줄 지 가지고 있다.
			// 이 데이터를 MeshData를 저장한다.
			for (UINT w = 0; w < Bone->mNumWeights; w++)
			{
				const UINT VertexId = Bone->mWeights[w].mVertexId; // ex) VertexId : 20978
				const float Weight = Bone->mWeights[w].mWeight; // ex) 20978번 Vertex에 이 Bone이 미치는 영향력이 0.025f다.
				constexpr float WeightThreshold = 0.015f; 
				if (Weight < WeightThreshold)
					continue;
				MeshData::VertexType & TargetVertex = InOutMeshes[MeshIndex]->Vertices[VertexId];
				Vector4 & TargetIndices = TargetVertex.Indices;
				Vector4 & TargetWeights = TargetVertex.Weights;

				// 최대 영향을 받을거 4개다.
				for (UINT v = 0; v < 4; v++)
				{
					if (TargetIndices.V[v] <= 0)
					{
						TargetIndices.V[v] = static_cast<float>(TargetBoneIndex);
						TargetWeights.V[v] = Weight;
						break;
					}
				}// for(V)
			}//for(w)
		} //for(boneIndex)
	}//for(MeshIndex)
}

void MeshConverter::WriteSkin(
	const wstring & InPath,
	const vector<BoneData*>& InBones,
	const vector<MeshData*>& InMeshes
)
{
	Path::CreateFolder(Path::GetDirectoryName(InPath));

	BinaryWriter * MeshDataWriter = new BinaryWriter();
	MeshDataWriter->Open(InPath);

	MeshDataWriter->WriteUint(InMeshes.size());
	for (const MeshData * MeshData : InMeshes)
	{
		MeshDataWriter->WriteString(MeshData->Name);
		MeshDataWriter->WriteString(MeshData->MaterialName);
		MeshDataWriter->WriteSTDVector<MeshData::VertexType>(MeshData->Vertices);
		MeshDataWriter->WriteSTDVector<UINT>(MeshData->Indices);
	}

	MeshDataWriter->WriteUint(InBones.size());
	for (const BoneData * BoneData : InBones)
	{
		MeshDataWriter->WriteInt(BoneData->Index);
		MeshDataWriter->WriteString(BoneData->Name);
		MeshDataWriter->WriteInt(BoneData->Parent);
		MeshDataWriter->WriteMatrix(BoneData->Transform);
		MeshDataWriter->WriteSTDVector<UINT>(BoneData->MeshIndices);
	}

	MeshDataWriter->Close();
	SAFE_DELETE(MeshDataWriter);
}

MeshData::VertexType MeshConverter::ReadSingleVertex( const aiMesh * Mesh, UINT VertexIndex, const aiMatrix4x4 & InMeshTransform )
{
	aiMatrix4x4 PreRotation;
	aiMatrix4x4::RotationY(PRE_Y_ROTATION, PreRotation);

	MeshData::VertexType Vertex;
	if (true)
	{
		const aiVector3D & LocalVertex = Mesh->mVertices[VertexIndex];
		const aiVector3D TransformedVertex = PreRotation * InMeshTransform * LocalVertex;
		// const aiVector3D TransformedVertex = LocalVertex;
		memcpy_s(&Vertex.Position, sizeof(Vector), &TransformedVertex, sizeof(Vector));
	}
	if (Mesh->HasTextureCoords(0) == true)
	{
		memcpy_s(&Vertex.UV, sizeof(Vector2D), Mesh->mTextureCoords[0] + VertexIndex, sizeof(Vector2D));
	}
	if (Mesh->HasVertexColors(0) == true)
	{
		memcpy_s(&Vertex.Color, sizeof(Color), Mesh->mColors[0] + VertexIndex, sizeof(Color));
	}
	if (Mesh->HasNormals() == true)
	{
		const aiVector3D & LocalNormal = Mesh->mNormals[VertexIndex];
		const aiVector3D TransformedNormal = PreRotation * InMeshTransform * LocalNormal;
		// const aiVector3D TransformedNormal = LocalNormal;
		memcpy_s(&Vertex.Normal, sizeof(Vector), &TransformedNormal, sizeof(Vector));
	}
	if (Mesh->HasTangentsAndBitangents() == true)
	{
		const aiVector3D & LocalTangent = Mesh->mTangents[VertexIndex];
		const aiVector3D TransformedTangent = PreRotation * InMeshTransform * LocalTangent;
		// const aiVector3D TransformedTangent = LocalTangent;
		memcpy_s(&Vertex.Tangent, sizeof(Vector), &TransformedTangent, sizeof(Vector));
	}
	else
	{
		Vertex.Tangent = {1,0,0};
	}
	return Vertex;
}

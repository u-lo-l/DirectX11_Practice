#include "Pch.h"
#include "Converter.h"
#include <fstream>

namespace Sdt
{
	Converter::Converter()
	 : Loader(nullptr), Scene(nullptr)
	{
		Loader = new Assimp::Importer();
	}

	Converter::~Converter()
	{
		Loader->FreeScene();
		SAFE_DELETE(Loader);
	}

	void Converter::ReadAiSceneFromFile( const wstring & InFileName )
	{
		FilePath = W_ASSET_PATH + InFileName;

		Scene = Loader->ReadFile(
			String::ToString(FilePath).c_str(),
			aiProcess_ConvertToLeftHanded
			| aiProcess_Triangulate
			| aiProcess_GenUVCoords
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
			| aiProcess_GenBoundingBoxes
		);

		ASSERT(Scene != nullptr, Loader->GetErrorString());
	}

	string Converter::ColorToJson( const Color & InColor )
	{
		return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
	}
	
#pragma region ExtractMaterial
	
	void Converter::ExportMaterial( const wstring & InSaveFileName, const string & InShaderFileName, bool InbOverwrite )
	{
		const wstring SaveFileName = W_MODEL_PATH + InSaveFileName + L".material";
		
		ReadMaterial(InShaderFileName);
		WriteMaterial(SaveFileName, InbOverwrite);
	}

	void Converter::ReadMaterial(const string & InShaderName)
	{
		Materials.resize(Scene->mNumMaterials, nullptr);
		for (UINT i = 0; i < Scene->mNumMaterials; i++)
		{
			const aiMaterial* Material = Scene->mMaterials[i];
			Materials[i] = new MaterialData();

			Materials[i]->Name = Material->GetName().C_Str();
			Materials[i]->ShaderName = InShaderName;
		
			aiColor4D color;
			Material->Get(AI_MATKEY_COLOR_AMBIENT, color);
			Materials[i]->Ambient = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			Materials[i]->Diffuse = Color(color.r, color.g, color.b, color.a);
		
			float shininess = 0.0f;
			Material->Get(AI_MATKEY_SHININESS, shininess);
			Material->Get(AI_MATKEY_COLOR_SPECULAR, color);
			Materials[i]->Specular = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			Materials[i]->Emissive = Color(color.r, color.g, color.b, color.a);

			Materials[i]->SetTextureFilesFromAiMaterialByTextureType(Material, aiTextureType_DIFFUSE);
			Materials[i]->SetTextureFilesFromAiMaterialByTextureType(Material, aiTextureType_SPECULAR);
			Materials[i]->SetTextureFilesFromAiMaterialByTextureType(Material, aiTextureType_NORMALS);
		}
	}

	void Converter::WriteMaterial( const wstring & InSaveFileName, bool InbOverwrite ) const
	{
		if (InbOverwrite == false)
		{
			if (Path::IsFileExist(InSaveFileName) == true)
				return;
		}

		string FolderName = String::ToString(Path::GetDirectoryName(InSaveFileName));
		string FileName = String::ToString(Path::GetFileName(InSaveFileName));

		Path::CreateFolders(FolderName);

		Json::Value Root;

		for (const MaterialData* Data : Materials)
		{
			Json::Value Value;
			Value["ShaderName"] = Data->ShaderName;

			Value["Ambient"] = ColorToJson(Data->Ambient);
			Value["Diffuse"] = ColorToJson(Data->Diffuse);
			Value["Specular"] = ColorToJson(Data->Specular);
			Value["Emissive"] = ColorToJson(Data->Emissive);

			for (const string & Name : Data->DiffuseFiles)
				Value["DiffuseMap"].append(SaveTextureAsFile(FolderName, Name));
			for (const string & Name : Data->SpecularFiles)
				Value["SpecularMap"].append(SaveTextureAsFile(FolderName, Name));
			for (const string & Name : Data->NormalFiles)
				Value["NormalMap"].append(SaveTextureAsFile(FolderName, Name));

			Root[Data->Name.c_str()] = Value;
			SAFE_DELETE(Data);
		}

		wstring SaveMaterialFileName = W_MATERIAL_PATH + Path::GetFileName(InSaveFileName);
		
		Json::StyledWriter JsonWriter;
		string TempStr = JsonWriter.write(Root);
		
		ofstream Ofstream;
		Ofstream.open(SaveMaterialFileName);
		Ofstream << TempStr;
		Ofstream.close();
	}

	string Converter::SaveTextureAsFile(const string & InSaveFolder, const string & InFileName) const
	{
		ID3D11Device * const Device = D3D::Get()->GetDevice();
		ID3D11DeviceContext * const Context = D3D::Get()->GetDeviceContext();
		
		ASSERT(InSaveFolder.empty() == false, "InSaveFolder is not valid");
		ASSERT(InFileName.empty() == false, "InFileName is not valid");

		const string Filename = Path::GetFileName(InFileName);
		const aiTexture * Texture = Scene->GetEmbeddedTexture(InFileName.c_str());
		if (Texture != nullptr)
		{
			if (Texture->mHeight == 0) // jpeg나 png처럼 압축된 형식의 image
			{
				// const BinaryWriter * BinWriter = new BinaryWriter(String::ToWString(InSaveFolder + filename));
				BinaryWriter * const BinWriter = new BinaryWriter();
				BinWriter->Open(InSaveFolder + Filename);
				BinWriter->WriteByte(Texture->pcData, Texture->mWidth);
				delete BinWriter;
				
				return InSaveFolder + Filename;
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
					String::ToWString(InSaveFolder + Filename).c_str()
				);
			CHECK(Hr < 0);
			return InSaveFolder + Filename;
		}
		else // FBX 내 임베디드 텍스처가 없는 경우, FBX 파일과 동일한 디렉토리에서 텍스처 파일을 찾음.
		{
			const string Directory = Path::GetDirectoryName(String::ToString(this->FilePath));
			string OriginPath = Directory + InFileName;
			String::Replace(&OriginPath, "\\", "/");

			if (Path::IsFileExist(OriginPath) == false)
				return "";

			const string Path = InSaveFolder + "/" + InFileName;
			CopyFileA(OriginPath.c_str(), Path.c_str(), false);
			return InSaveFolder + Path::GetFileName(Path);
		}
	}
	
#pragma endregion
	
#pragma region ExtractMesh
	
	void Converter::ExportMesh( const wstring & InSaveFileName )
	{
		wstring FullFileName = W_MODEL_PATH + InSaveFileName + L".mesh";
		ReadBoneData(Scene->mRootNode, -1, -1);
		ReadMeshData();

		//
		ReadSkinData();
		//

		ofstream Ofstream;
		Ofstream.open("../Vertices.csv");
		Ofstream << "NAME, X, Y, Z, W, SUM" "\n";
		for ( MeshData * MeshData : this->Meshes )
		{
			for (const MeshData::VertexType & vertex : MeshData->Vertices)
			{
				Ofstream << "Pos, " << vertex.Position.X << "," << vertex.Position.Y << "," << vertex.Position.Z << "," << " , " << "\n";
				Ofstream << "Ind, " << vertex.Indices[0] << "," << vertex.Indices[1] << "," << vertex.Indices[2] << "," << vertex.Indices[3]<< ", \n";
				Ofstream << "Wgh, " << vertex.Weights[0] << "," << vertex.Weights[1] << "," << vertex.Weights[2] << "," << vertex.Weights[3]<< "," << vertex.Weights[0] + vertex.Weights[1] + vertex.Weights[2] + vertex.Weights[3]<< "\n";
			}
		}
		Ofstream.close();
		
		WriteMesh(FullFileName);

		// 
		// ofstream Ofstream;
		// Ofstream.open("../BoneMat.csv");
		// for ( const auto & p : BoneMatrix)
		// {
		// 	Ofstream << p.first << "," << p.second.M41 << "," << p.second.M42 << "," << p.second.M43 << "\n";
		// }
		// Ofstream.close();
		//
		// Ofstream.open("../OffsetMat.csv");
		// for ( const auto & p : OffsetMatrix)
		// {
		// 	Ofstream << p.first << "," << p.second.M41 << "," << p.second.M42 << "," << p.second.M43 << "\n";
		// }
		// Ofstream.close();
		//

		//
		
		//
	}
	
	void Converter::ReadBoneData( const aiNode * InNode, int InIndex, int InParent )
	{
		BoneData * Bone = new BoneData();
		Bone->Index = InIndex;
		Bone->Parent = InParent;
		Bone->Name = InNode->mName.C_Str();
		
		Bone->Transform = InNode->mTransformation;
		Bone->Transform.Transpose();

		Matrix ParentMatrix = Matrix::Identity;
		if (InParent >= 0)
		{
			// 이미 Transpose된 부모 bone의 local-coord Transform.
			ParentMatrix = Bones[InParent]->Transform;
		}
		Bone->Transform = Bone->Transform * ParentMatrix;
		Bones.push_back(Bone);

		//
		// BoneMatrix.push_back(make_pair(Bone->Name, Bone->Transform));
		//

		const UINT MeshCount = InNode->mNumMeshes;
		Bone->MeshIndices.reserve(MeshCount);
		for (UINT i	= 0 ; i < MeshCount ; i++)
		{
			Bone->MeshIndices.push_back(InNode->mMeshes[i]);
		}

		const UINT ChildBoneCount = InNode->mNumChildren;
		for (UINT i = 0; i < ChildBoneCount ; i++)
		{
			ReadBoneData(InNode->mChildren[i], Bones.size(), InIndex);
		}
	}

	void Converter::ReadMeshData()
	{
		const UINT MeshCount = Scene->mNumMeshes;
		Meshes.resize(MeshCount);
		for (UINT i = 0; i < MeshCount; i++)
		{
			Meshes[i] = new MeshData();
			const aiMesh * const AiMesh = Scene->mMeshes[i];

			// Read Material Data
			const UINT MatIndex = AiMesh->mMaterialIndex;
			Meshes[i]->MaterialName = Scene->mMaterials[MatIndex]->GetName().C_Str();

			// Read Vertices Data
			const UINT VerticesCount = AiMesh->mNumVertices;
			Meshes[i]->Vertices.reserve(VerticesCount);
			for (UINT v = 0; v < VerticesCount; v++)
			{
				Meshes[i]->Vertices.emplace_back(ThisClass::ReadSingleVertexDataFromAiMesh(AiMesh, v));
			}

			// Read Indices Data
			const UINT FacesCount = AiMesh->mNumFaces;
			for (UINT f = 0; f < FacesCount; f++)
			{
				const aiFace & Face = AiMesh->mFaces[f];
				const UINT IndicesCount = Face.mNumIndices;
				for (UINT k = 0; k < IndicesCount; k++)
				{
					Meshes[i]->Indices.push_back(Face.mIndices[k]);
				}
			}
		}
	}

	MeshData::VertexType Converter::ReadSingleVertexDataFromAiMesh( const aiMesh * Mesh, UINT VertexIndex )
	{
		MeshData::VertexType Vertex;
		memcpy_s(&Vertex.Position, sizeof(Vector), Mesh->mVertices + VertexIndex, sizeof(Vector));

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
			memcpy_s(&Vertex.Normal, sizeof(Vector), Mesh->mNormals + VertexIndex, sizeof(Vector));
		}
		if (Mesh->HasTangentsAndBitangents() == true)
		{
			memcpy_s(&Vertex.Tangent, sizeof(Vector), Mesh->mTangents + VertexIndex, sizeof(Vector));
		}
		return Vertex;
	}

	void Converter::WriteMesh( const wstring & InSaveFileName ) const
	{
		Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));
		BinaryWriter * BinWriter = new BinaryWriter();
		BinWriter->Open(InSaveFileName);

		BinWriter->WriteUint(Bones.size());
		for (const BoneData * BoneData : Bones)
		{
			BinWriter->WriteUint(BoneData->Index);
			BinWriter->WriteString(BoneData->Name);
			BinWriter->WriteInt(BoneData->Parent);
			BinWriter->WriteMatrix(BoneData->Transform);

			const UINT MeshIndexCount = BoneData->MeshIndices.size();
			BinWriter->WriteUint(MeshIndexCount);
			if (MeshIndexCount > 0)
				BinWriter->WriteByte(&(BoneData->MeshIndices[0]), MeshIndexCount * sizeof(UINT));
			SAFE_DELETE(BoneData);
		}

		BinWriter->WriteUint(Meshes.size());
		for (const MeshData * MeshData : Meshes)
		{
			BinWriter->WriteString(MeshData->Name);
			BinWriter->WriteString(MeshData->MaterialName);

			const UINT VerticesCount = MeshData->Vertices.size();
			BinWriter->WriteUint(VerticesCount);
			if (VerticesCount > 0)
				BinWriter->WriteByte(&(MeshData->Vertices[0]), VerticesCount * sizeof(MeshData::VertexType));

			const UINT IndicesCount = MeshData->Indices.size();
			BinWriter->WriteUint(IndicesCount);
			if (IndicesCount > 0)
				BinWriter->WriteByte(&(MeshData->Indices[0]), IndicesCount * sizeof(UINT));	
			SAFE_DELETE(MeshData);
		}
		
		BinWriter->Close();
		SAFE_DELETE(BinWriter);
	}

	void Converter::ReadSkinData()
	{
		const UINT MeshCount = Scene->mNumMeshes;
		for (UINT i = 0; i < MeshCount; i++)
		{
			aiMesh * const mesh = Scene->mMeshes[i];
			// Bone이아니다 -> 스키닝이 될 아이가 아니다.
			if (mesh->HasBones() == false) 
			{
				continue;
			}

			const UINT BoneCount = mesh->mNumBones;
			for (UINT boneIndex = 0; boneIndex < BoneCount; boneIndex++)
			{
				aiBone * Bone = mesh->mBones[boneIndex];
				const char * boneName = Bone->mName.C_Str();

				UINT TargetBoneIndex = 0;
				for (UINT j = 0; j < Bones.size(); j++)
				{
					if (Bones[j]->Name == boneName)
					{
						TargetBoneIndex = j;
						break;
					}
				}

				for (UINT w = 0; w < Bone->mNumWeights; w++)
				{
					const UINT VertexId = Bone->mWeights[w].mVertexId;
					const float Weight = Bone->mWeights[w].mWeight;

					MeshData * meshdata =  this->Meshes[i];
					Vector4 & Indices = meshdata->Vertices[VertexId].Indices;
					Vector4 & Weights = meshdata->Vertices[VertexId].Weights;

					// 최대 영향을 받을거 4개다.

					UINT v = 0;
					for (; v < 4; v++)
					{
						// 
						if (Indices.V[v] <= 0)
						{
							Indices.V[v] = static_cast<float>(TargetBoneIndex);
							Weights.V[v] = Weight;
							break;
						}
					}// for(V)
				}//for(w)
			} //for(boneIndex)
		}//for(i)
	}

#pragma endregion
	
#pragma region Extract Animation

	void Converter::ExportAnimation( const string & InSaveFileName, int InClipIndex )
	{
		wstring saveFileName = W_MODEL_PATH;
		if (InClipIndex < 0)
		{
			vector<string> ClipNames;
			ReadClips(ClipNames);

			for (UINT i = 0; i < ClipNames.size(); i++)
			{
				ReadClipData(Scene->mAnimations[i]);
			}
			
			return ;
		}
		ReadClipData(Scene->mAnimations[InClipIndex]);
	}

	void Converter::ReadClips( vector<string> & OutClips )
	{
		ASSERT(Scene != nullptr, "aiScene is not valid");

		const UINT ClipCount = this->Scene->mNumAnimations;
		for (int i = 0; i < ClipCount; i++)
		{
			OutClips.push_back( Scene->mAnimations[i]->mName.C_Str());
		}
	}

	void Converter::ReadClipData( const aiAnimation * InAnimation )
	{
		ClipData Data = {
			InAnimation->mName.C_Str(),
			static_cast<float>(InAnimation->mDuration),
			static_cast<float>(InAnimation->mTicksPerSecond),
		};

		// Channel이란 weight를 줄 bone 번호
		const UINT ChannelsCount = InAnimation->mNumChannels;
		// vector<string> Channels;
		// Channels.reserve(ChannelsCount);
		for (UINT i = 0; i < ChannelsCount; i++)
		{
			// aiNodeAnim 은 PositionKey, RotationKey, ScalingKey등 KeyFrame Animation의 Key를 갖는다.
			const aiNodeAnim * const NodeAnim = InAnimation->mChannels[i];
			// Channels.emplace_back(NodeAnim->mNodeName.C_Str());
			
			// ClipNodeData * NodeData = new ClipNodeData();
			// NodeData->BoneName = NodeAnim->mNodeName.C_Str();

			vector<KeyVecData> VecDatas;
			vector<KeyVecData> ScaleDatas;
			vector<KeyQuatData> QuatData;

			ReadPosKeys(VecDatas, NodeAnim);
			ReadScaleKeys(ScaleDatas, NodeAnim);
			ReadRotKeys(QuatData, NodeAnim);
		}
	}

	void Converter::ReadPosKeys( vector<KeyVecData> & OutPosKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT PosKeyCount = InNodeAnim->mNumPositionKeys;
		OutPosKeys.reserve(PosKeyCount);
		for (UINT i = 0; i < PosKeyCount; i++)
		{
			const aiVectorKey & PosKey =  InNodeAnim->mPositionKeys[i];
			KeyVecData Key;
			Key.Time = static_cast<float>(PosKey.mTime);
			memcpy(&Key.Values, &PosKey.mValue, sizeof(Vector));
			OutPosKeys.push_back(Key);
		}
	}

	void Converter::ReadScaleKeys( vector<KeyVecData> & OutScaleKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT ScaleKeyCount = InNodeAnim->mNumScalingKeys;
		OutScaleKeys.reserve(ScaleKeyCount);
		for (UINT i = 0; i < ScaleKeyCount; i++)
		{
			const aiVectorKey & PosKey =  InNodeAnim->mScalingKeys[i];
			KeyVecData Key;
			Key.Time = static_cast<float>(PosKey.mTime);
			memcpy(&Key.Values, &PosKey.mValue, sizeof(Vector));
			OutScaleKeys.push_back(Key);
		}
	}

	void Converter::ReadRotKeys( vector<KeyQuatData> & OutRotKeys, const aiNodeAnim * InNodeAnim )
	{
		const UINT RotKeyCount = InNodeAnim->mNumRotationKeys;
		OutRotKeys.reserve(RotKeyCount);
		for (UINT i = 0; i < RotKeyCount; i++)
		{
			const aiQuatKey & RotKey =  InNodeAnim->mRotationKeys[i];
			KeyQuatData Key;
			Key.Time = static_cast<float>(RotKey.mTime);
			memcpy(&Key.Values, &RotKey.mValue, sizeof(Quaternion));
			OutRotKeys.push_back(Key);
		}
	}

#pragma endregion
}

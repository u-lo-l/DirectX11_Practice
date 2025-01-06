#include "Pch.h"
#include "Converter.h"
#include <string>
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

	void Converter::ReadFile( const wstring & InFileName )
	{
		// string FileName = String::ToString(InFileName);
		FilePath = L"../../_Assets/" + InFileName;

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

	string Converter::ColorToJson( const Color & InColor ) const
	{
		return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
	}

	void Converter::ExportMaterial( const wstring & InSaveFileName, bool InbOverwrite )
	{
		const wstring SaveFileName = L"../../_Materials/" + InSaveFileName + L".material";
		
		ReadMaterial();
		WriteMaterial(SaveFileName, InbOverwrite);
	}

	void Converter::ReadMaterial()
	{
		// printf("mNumMaterials = %d\n", Scene->mNumMaterials);
		for (UINT i = 0; i < Scene->mNumMaterials; i++)
		{
			const aiMaterial* Material = Scene->mMaterials[i];
			MaterialData * MatData = nullptr;
			MatData = new MaterialData();

			MatData->Name = Material->GetName().C_Str();
			MatData->ShaderName = "";
		
			aiColor4D color;
			Material->Get(AI_MATKEY_COLOR_AMBIENT, color);
			MatData->Ambient = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			MatData->Diffuse = Color(color.r, color.g, color.b, color.a);
		
			float shininess = 0.0f;
			Material->Get(AI_MATKEY_SHININESS, shininess);
			Material->Get(AI_MATKEY_COLOR_SPECULAR, color);
			MatData->Specular = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			MatData->Emissive = Color(color.r, color.g, color.b, color.a);

			MatData->SetTextureFiles(Material, aiTextureType_DIFFUSE);
			MatData->SetTextureFiles(Material, aiTextureType_SPECULAR);
			MatData->SetTextureFiles(Material, aiTextureType_NORMALS);
			
			Materials.push_back(MatData);
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
			Json::Value ShaderName;
			ShaderName["ShaderName"] = Data->ShaderName;

			Json::Value Color;
			Color["Ambient"] = ColorToJson(Data->Ambient);
			Color["Diffuse"] = ColorToJson(Data->Diffuse);
			Color["Specular"] = ColorToJson(Data->Specular);
			Color["Emissive"] = ColorToJson(Data->Emissive);

			Json::Value Textures;

			for (const string & Name : Data->DiffuseFiles)
				Textures["Diffuse"].append(SaveTexture(FolderName, Name));
			for (const string & Name : Data->SpecularFiles)
				Textures["Specular"].append(SaveTexture(FolderName, Name));
			for (const string & Name : Data->NormalFiles)
				Textures["Normal"].append(SaveTexture(FolderName, Name));
		
			
			Root[Data->Name.c_str()].append(ShaderName);
			Root[Data->Name.c_str()].append(Color);
			Root[Data->Name.c_str()].append(Textures);
		}
		
		Json::StyledWriter JsonWriter;
		string TempStr = JsonWriter.write(Root);
		
		ofstream Ofstream;
		Ofstream.open(InSaveFileName);
		Ofstream << TempStr;
		Ofstream.close();
	}
#pragma region ExtractMesh
	void Converter::ExportMesh( const wstring & InSaveFileName )
	{
		wstring FullFileName = L"../../_Models/" + InSaveFileName + L".mesh";
		ReadBoneData(Scene->mRootNode, -1, -1);
	}

	void Converter::ReadBoneData( const aiNode * InRootNode, int InIndex, int InParent )
	{
		
	}


#pragma endregion

	string Converter::SaveTexture(const string & InSaveFolder, const string & InFileName) const
	{
		printf("InSaveFolder = %s\n", InSaveFolder.c_str());
		printf("InFileName = %s\n", InFileName.c_str());
		ASSERT(InSaveFolder.empty() == false, "InSaveFolder is not valid");
		ASSERT(InFileName.empty() == false, "InFileName is not valid");
		
		string filename = Path::GetFileName(InFileName);
		const aiTexture * Texture = Scene->GetEmbeddedTexture(InFileName.c_str());
		if (Texture != nullptr)
		{
			if (Texture->mHeight == 0) // jpeg나 png처럼 압축된 형식의 image
			{
				const BinaryWriter * BinWriter = new BinaryWriter(String::ToWString(InSaveFolder + filename));
				BinWriter->Write(Texture->pcData, Texture->mWidth);
				delete BinWriter;
				
				return filename;
			}
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
			HRESULT Hr = D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, &SubResourceData, &SavingTexture);
			printf("%d\n", Hr);
			CHECK(D3DX11SaveTextureToFileA(D3D::Get()->GetDeviceContext(),
											SavingTexture,
											D3DX11_IFF_PNG,
											(InSaveFolder + filename).c_str()
											) >= 0);
			
			return filename;
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
			return Path::GetFileName(Path);
		}
	}

}

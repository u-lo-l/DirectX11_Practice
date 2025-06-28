#include "framework.h"
#include "CStaticMesh.h"

#include <fstream>

CStaticMesh::CStaticMesh(const wstring& InModelName)
{
	this->Name = InModelName;
	const wstring ModelPath = W_MODEL_PATH + InModelName + L".model";
	ASSERT(Path::IsFileExist(ModelPath) == true, String::Format("%s | Not Found", ModelPath).c_str());

	Json::Value Root;
	{
		ifstream Ifs;
		Ifs.open(ModelPath);
		Ifs >> Root;
		Ifs.close();
	}

	ReadTransform(Root);
	ReadMaterial(Root);
	ReadSubMeshes(Root);

	for (StaticMeshSubset * Subset : MeshSubsets)
	{
		RenderManager::Get()->AddRenderable(Subset);
	}
}

CStaticMesh::~CStaticMesh()
{
	SAFE_DELETE(Tf);
}

void CStaticMesh::Tick()
{
}


void CStaticMesh::ReadTransform(Json::Value::const_iterator::reference Root) const
{
	const Vector Position = Helper::JsonToVector3(Root["Transform"]["Position"].asString());
	const Vector Euler = Helper::JsonToVector3(Root["Transform"]["Rotation"].asString());
	const Vector Scale = Helper::JsonToVector3(Root["Transform"]["Scale"].asString());
	const Quaternion Rotation = Quaternion::CreateFromEulerAngleInRadian(Euler);

	Tf->SetTRS(Position, Rotation, Scale);
}

void CStaticMesh::ReadMaterial(Json::Value::const_iterator::reference Root)
{
	const wstring MaterialFileName = String::ToWString(Root["File"]["Material"].asString());
	const wstring FullPath = W_MATERIAL_PATH + MaterialFileName + L"/" + MaterialFileName + L".material";
	ASSERT(Path::IsFileExist(FullPath) == true, String::Format("%s | Not Found", FullPath).c_str());
	
	Json::Value MaterialRoot;

	{
		ifstream Stream;
		Stream.open(FullPath);
		Stream >> MaterialRoot;
		Stream.close();
	}
	
	const Json::Value::Members Members = MaterialRoot.getMemberNames();
	for (const Json::String & Name : Members)
	{
		Json::Value Value = MaterialRoot[Name];
		if (Materials.find(Name) != Materials.cend())
			continue;
		string MaterialName = String::ToString(this->Name) + "_" + Name;
		Material * Mat = new MaterialType(Value, MaterialName);
		Materials.insert({ Name, Mat });
	}
}

void CStaticMesh::ReadSubMeshes(Json::Value::const_iterator::reference Root)
{
	const wstring MeshFileName = String::ToWString(Root["File"]["Mesh"].asString());
	const wstring FullPath = W_MODEL_PATH + MeshFileName + L"/Mesh/" + MeshFileName + L".mesh";
	ASSERT(Path::IsFileExist(FullPath) == true, String::Format("%s | Not Found", FullPath).c_str());
	
	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);
	ReadSubMeshes(BinReader);
	BinReader->Close();
	SAFE_DELETE(BinReader);
}

void CStaticMesh::ReadSubMeshes(const BinaryReader* InBinReader)
{
	const UINT MeshCount = InBinReader->ReadUint();
	this->MeshSubsets.resize( MeshCount );

	for (UINT i = 0; i < MeshCount; i++)
	{
		StaticMeshSubset::MeshSubsetDesc Desc = {};
		string MeshName = InBinReader->ReadString();
		const string MaterialName = InBinReader->ReadString();
		InBinReader->ReadSTDVector<VertexType>(Desc.Vertices);
		InBinReader->ReadSTDVector<UINT>(Desc.Indices);
		
		if (MeshName.empty() == true)
			Desc.Name = "Mesh #" + to_string(i) + " for " + MaterialName;
		else
			Desc.Name = MeshName;
		Desc.Material = this->Materials.at(MaterialName); 
		
		this->MeshSubsets[i] = new StaticMeshSubset(Desc);
		this->MeshSubsets[i]->GetTransform()->SetParent(this->Tf);
	}
}

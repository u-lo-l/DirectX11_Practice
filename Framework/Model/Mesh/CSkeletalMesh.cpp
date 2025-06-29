#include "framework.h"
#include <iostream>
#include <fstream>

CSkeletalMesh::CSkeletalMesh(const wstring& InModelName)
 : Tf(new Transform()), Skeleton(nullptr)
{
	this->Name = InModelName;
	const wstring ModelPath = W_MODEL_PATH + InModelName + L".model";
	ASSERT(Path::IsFileExist(ModelPath) == true, String::Format("%s | Not Found", ModelPath).c_str());

	ifstream Ifs;
	Ifs.open(ModelPath);
		Json::Value Root;
		Ifs >> Root;
	Ifs.close();

	ReadTransform(Root);
	ReadMaterial(Root);
	ReadSubMeshesAndBones(Root);

	for (SkeletalMeshSubset * Subset : MeshSubsets)
	{
		Subset->SetSkeletal(this->Skeleton);
		RenderManager::Get()->AddRenderable(Subset);
	}
}

CSkeletalMesh::~CSkeletalMesh()
{
	SAFE_DELETE(Tf);
	SAFE_DELETE(Skeleton);
}

void CSkeletalMesh::Tick()
{
	// TODO
}

CSkeletal* CSkeletalMesh::GetSkeletal() const
{
	return Skeleton;
}

Transform* CSkeletalMesh::GetTransform() const
{
	return Tf;
}

void CSkeletalMesh::ReadTransform(Json::Value::const_iterator::reference Root) const
{
	const Vector Position = Helper::JsonToVector3(Root["Transform"]["Position"].asString());
	const Vector Euler = Helper::JsonToVector3(Root["Transform"]["Rotation"].asString());
	const Vector Scale = Helper::JsonToVector3(Root["Transform"]["Scale"].asString());
	const Quaternion Rotation = Quaternion::CreateFromEulerAngleInRadian(Euler);

	Tf->SetTRS(Position, Rotation, Scale);
}


void CSkeletalMesh::ReadMaterial(const Json::Value::const_iterator::reference Root)
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
		const string ShaderName = Value["ShaderName"].asString();
		Material * Mat = new MeshMaterial(Value, MaterialName, ShaderName);
		Materials.insert({ Name, Mat });
	}
}

void CSkeletalMesh::ReadSubMeshesAndBones(const Json::Value::const_iterator::reference Root)
{
	const wstring MeshFileName = String::ToWString(Root["File"]["Mesh"].asString());
	const wstring FullPath = W_MODEL_PATH + MeshFileName + L"/Mesh/" + MeshFileName + L".mesh";
	ASSERT(Path::IsFileExist(FullPath) == true, String::Format("%s | Not Found", FullPath).c_str());
	
	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);
		vector<CBone *> Bones;
		ReadSubMeshes(BinReader);
		ReadSkeletalData(BinReader, Bones);
		Skeleton = new CSkeletal(Bones);
	BinReader->Close();
	SAFE_DELETE(BinReader);
}


void CSkeletalMesh::ReadSubMeshes(const BinaryReader* InBinReader)
{
	const UINT MeshCount = InBinReader->ReadUint();
	this->MeshSubsets.resize( MeshCount );

	for (UINT i = 0; i < MeshCount; i++)
	{
		SkeletalMeshSubset::MeshSubsetDesc Desc = {};
		string MeshName = InBinReader->ReadString();
		const string MaterialName = InBinReader->ReadString();
		InBinReader->ReadSTDVector<VertexType>(Desc.Vertices);
		InBinReader->ReadSTDVector<UINT>(Desc.Indices);
		
		if (MeshName.empty() == true)
			Desc.Name = "Mesh #" + to_string(i) + " for " + MaterialName;
		else
			Desc.Name = MeshName;
		Desc.Material = this->Materials.at(MaterialName); 
		
		this->MeshSubsets[i] = new SkeletalMeshSubset(Desc);
		this->MeshSubsets[i]->GetTransform()->SetParent(this->Tf);
	}
}

void CSkeletalMesh::ReadSkeletalData(const BinaryReader* InBinReader, vector<CBone *>& OutBones)
{
	SAFE_DELETE(this->Skeleton);

	const UINT BoneCount = InBinReader->ReadUint();
	if (BoneCount == 0)
		return;

	// STEP 01 : 모든 Bone 정보 우선 읽기
	OutBones.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; i++)
	{
		CBone::BoneDesc Desc;
		Desc.Index = InBinReader->ReadInt();
		Desc.BoneName = InBinReader->ReadString();
		Desc.ParentIndex = InBinReader->ReadInt();
		Desc.RootTransform = InBinReader->ReadMatrix();
		Desc.OffsetMatrix = Matrix::Invert(Desc.RootTransform, true);

		const UINT MeshCount = InBinReader->ReadUint();
		vector<UINT> AttachedMeshes;
		if (MeshCount > 0)
		{
			AttachedMeshes.assign(MeshCount, 0);
			void * Ptr = AttachedMeshes.data();
			InBinReader->ReadByte(&Ptr, sizeof(UINT) * MeshCount);
		}
		
		OutBones[i] = new CBone(Desc);
		OutBones[i]->SetAttachedMeshIndices(AttachedMeshes);
	}

	// STEP 02 : 읽은 Bone들의 계층구조 정리 및 Mesh 연결
	for (CBone * Bone : OutBones)
	{
		if (Bone->IsRootBone() == true) // root bone-node
			continue;;
		
		int ParentIndex = Bone->GetParentIndex();
		Bone->SetParentBone(OutBones[ParentIndex],ParentIndex);
		Bone->GetParentBone()->AddChildBone(Bone);

		for (const UINT MeshIndex : Bone->GetAttachedMeshIndices())
		{
			MeshSubsets[MeshIndex]->SetTransform(Bone->GetRootTransform());
		}
	}
}



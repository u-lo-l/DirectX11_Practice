#include "framework.h"
#include <iostream>
#include "CSkeletalMesh.h"

CSkeletalMesh::CSkeletalMesh(const wstring& InModelName)
 : Tf(new Transform()), Skeleton(nullptr)
{
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

	CB_Matrix = new ConstantBuffer(
		ShaderType::VertexShader,
		0,
		nullptr,
		"Transform Matrix",
		sizeof(WVPIDesc),
		false
	);
	CB_Light = new ConstantBuffer(
		ShaderType::VP,
		1,
		nullptr,
		"Light Color, Light Direction",
		sizeof(DirectionalLightDesc),
		false
	);
}

CSkeletalMesh::~CSkeletalMesh()
{
	SAFE_DELETE(Tf);
	SAFE_DELETE(Skeleton);
	SAFE_DELETE(CB_Matrix);
	SAFE_DELETE(CB_Light);
}

void CSkeletalMesh::Tick()
{
	WVPIDesc MatrixData;
	MatrixData.World = Tf->GetMatrix();
	MatrixData.View = Context::Get()->GetViewMatrix();
	MatrixData.Projection = Context::Get()->GetProjectionMatrix();
	MatrixData.ViewInverse = Matrix::Invert(MatrixData.View, true);
	CB_Matrix->UpdateData(&MatrixData, sizeof(WVPIDesc));

	DirectionalLightDesc LightDesc;
	LightDesc.LightColor = Context::Get()->GetLightColor();
	LightDesc.LightDirection = Context::Get()->GetLightDirection();
	CB_Light->UpdateData(&LightDesc, sizeof(DirectionalLightDesc));
	
	for (MeshSubset * Subset : MeshSubsets)
		Subset->Tick();
}

void CSkeletalMesh::Render()
{
	CB_Matrix->BindToGPU(); // 0
	CB_Light->BindToGPU();	// 1
	Skeleton->BindToGPU(); // 2, 3
	
	for (MeshSubset * Subset : MeshSubsets)
		Subset->Render();
}

CSkeletal* CSkeletalMesh::GetSkeletal() const
{
	return Skeleton;
}

void CSkeletalMesh::ReadTransform(Json::Value::const_iterator::reference Root)
{
	const Vector Position = Helper::JsonToVector3(Root["Transform"]["Position"].asString());
	const Vector Euler = Helper::JsonToVector3(Root["Transform"]["Rotation"].asString());
	const Vector Scale = Helper::JsonToVector3(Root["Transform"]["Scale"].asString());
	const Quaternion Rotation = Quaternion::CreateFromEulerAngleInRadian(Euler);

	Tf->SetTRS(Position, Rotation, Scale);
}


void CSkeletalMesh::ReadMaterial(const Json::Value::const_iterator::reference Root)
{
	const wstring MaterialFileName = String::ToWString(Root["File"]["Materials"].asString());
	const wstring FullPath = W_MATERIAL_PATH + MaterialFileName + L"/" + MaterialFileName + L".material";
	ASSERT(Path::IsFileExist(FullPath) == true, String::Format("%s | Not Found", FullPath).c_str());
	
	ifstream Stream;
	Stream.open(FullPath);
		Json::Value MaterialRoot;
		Stream >> MaterialRoot;
	Stream.close();
	
	const Json::Value::Members Members = MaterialRoot.getMemberNames();
	for (const Json::String & Name : Members)
	{
		Material<VertexType> * MatData = new Material<VertexType>();
		Json::Value Value = MaterialRoot[Name];

		if (Materials.find(Name) != Materials.cend())
			continue;
		
		ReadShaderName(Value, MatData);
		ReadColor(Value, MatData);
		ReadTextures(Value, MatData);
	
		Materials[Name] = MatData;
	}
}

void CSkeletalMesh::ReadSubMeshesAndBones(const Json::Value::const_iterator::reference Root)
{
	const wstring MeshFileName = String::ToWString(Root["File"]["Mesh"].asString());
	const wstring FullPath = W_MODEL_PATH + MeshFileName + L"/Mesh/" + MeshFileName + L".mesh";
	ASSERT(Path::IsFileExist(FullPath) == true, String::Format("%s | Not Found", FullPath).c_str());
	
	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);
		ReadSubMeshes(BinReader);
		vector<CBone *> Bones;
		ReadSkeletalData(BinReader, Bones);
		Skeleton = new CSkeletal(Bones);
	BinReader->Close();
	SAFE_DELETE(BinReader);
}

void CSkeletalMesh::ReadShaderName(const Json::Value& Value, Material<VertexType>* OutMatData, bool bUseAnimation)
{
	string ShaderName = Value["ShaderName"].asString();
	if (ShaderName == "")
		ShaderName = "Mesh/Mesh.hlsl";
	
	vector<D3D_SHADER_MACRO> Macros = {};
	if(bUseAnimation)
		Macros.push_back({"USE_ANIMATION", ""});
	Macros.push_back({nullptr, nullptr});
	OutMatData->SetShader(
		String::ToWString(ShaderName),
		Macros.data()
	);
}

void CSkeletalMesh::ReadColor(const Json::Value& Value, Material<VertexType>* MatData)
{
	MatData->SetAmbient(Helper::JsonToColor(Value["Ambient"].asString()));
	MatData->SetDiffuse(Helper::JsonToColor(Value["Diffuse"].asString()));
	MatData->SetSpecular(Helper::JsonToColor(Value["Specular"].asString()));
	MatData->SetEmissive(Helper::JsonToColor(Value["Emissive"].asString()));
}

void CSkeletalMesh::ReadTextures(const Json::Value& Value, Material<VertexType>* MatData)
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

void CSkeletalMesh::ReadSubMeshes(const BinaryReader* InBinReader)
{
	const UINT MeshCount = InBinReader->ReadUint();
	this->MeshSubsets.resize( MeshCount );

	for (UINT i = 0; i < MeshCount; i++)
	{
		vector<VertexType> Vertices;
		vector<UINT> Indices;
		MeshSubset::MeshSubsetDesc Desc = {};
		string MeshName = InBinReader->ReadString();
		const string MaterialName = InBinReader->ReadString();
		if (MeshName.empty() == true)
			Desc.Name = "Mesh #" + to_string(i) + " for " + MaterialName;
		else
			Desc.Name = MeshName;
		Desc.pMaterialData = this->Materials.at(MaterialName); 
		
		const UINT VertexCount = InBinReader->ReadUint();
		if (VertexCount > 0)
		{
			Vertices.resize(VertexCount);
			void * Ptr = Vertices.data();
			InBinReader->ReadByte(&Ptr, sizeof(VertexType) * VertexCount);
		}
		Desc.pVertices = &Vertices;

		const UINT IndexCount = InBinReader->ReadUint();
		if (IndexCount > 0)
		{
			Indices.resize(IndexCount);
			void * Ptr = Indices.data();
			InBinReader->ReadByte(&Ptr, sizeof(UINT) * IndexCount);
		}
		Desc.pIndices = &Indices;

		this->MeshSubsets[i] = new MeshSubset(Desc);
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



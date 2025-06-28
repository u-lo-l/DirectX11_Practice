#include "framework.h"
#include "MeshMaterial.h"

MeshMaterial::MeshMaterial
(
	const Json::Value& InValue,
	const string & InMaterialName
)	: Material( InValue, InMaterialName, MaterialType::Mesh )
{
	this->PerMaterialData.Ambient = Helper::JsonToColor(InValue["Ambient"].asString());
	this->PerMaterialData.Diffuse = Helper::JsonToColor(InValue["Diffuse"].asString());
	this->PerMaterialData.Specular = Helper::JsonToColor(InValue["Specular"].asString());
	this->PerMaterialData.Metallic = stof(InValue["Metallic"].asString());
	this->PerMaterialData.Glossiness = stof(InValue["Roughness"].asString());
	this->bTransparent = static_cast<bool>(stoi(InValue["Transparent"].asString()));
	
	// Read Textures
	wstring TexturePath;
	TexturePath = String::ToWString(InValue["DiffuseMap"].asString());
	if (TexturePath.empty() == false)
	{
		this->DiffuseTex = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseDiffuseTexture;
	}
	TexturePath = String::ToWString(InValue["SpecularMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->SpecularMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseSpecularTexture;
	}
	TexturePath = String::ToWString(InValue["NormalMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->NormalMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseNormalTexture;
	}
	TexturePath = String::ToWString(InValue["ShininessMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->ShininessMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseShininessTexture;
	}
	TexturePath = String::ToWString(InValue["AlbedoMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->AlbedoMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseAlbedoTexture;
	}
	TexturePath = String::ToWString(InValue["MetallicMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->MetallicMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseMetallicTexture;
	}
	TexturePath = String::ToWString(InValue["RoughnessMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->RoughnessMap = new Texture(TexturePath, true);
		this->PerMaterialData.TextureUsageFlag |= bUseRoughnessTexture;
	}

	CB_PerMaterial = new ConstantBuffer(
		ShaderType::PixelShader,
		0,
		&PerMaterialData,
		sizeof(PerMaterialDesc),
		false
	);
}

MeshMaterial::~MeshMaterial()
{
	SAFE_DELETE(NormalMap);
	SAFE_DELETE(DiffuseTex);
	SAFE_DELETE(SpecularMap);
	SAFE_DELETE(ShininessMap);
	SAFE_DELETE(AlbedoMap);
	SAFE_DELETE(MetallicMap);
	SAFE_DELETE(RoughnessMap);
}

void MeshMaterial::Tick()
{
	if (bDirty == false)
		return;
	CB_PerMaterial->UpdateData(&PerMaterialData, sizeof(PerMaterialDesc));
	bDirty = false;
}

void MeshMaterial::BindToGpu(int RegisterIndex) const
{
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(RegisterIndex);
	
	if (NormalMap != nullptr)
		NormalMap->BindToGPU(0);
	
	if (DiffuseTex != nullptr)
		DiffuseTex->BindToGPU(1);
	if (SpecularMap != nullptr)
		SpecularMap->BindToGPU(2);
	if (ShininessMap != nullptr)
		ShininessMap->BindToGPU(3);

	if (AlbedoMap != nullptr)
		AlbedoMap->BindToGPU(4);
	if (MetallicMap != nullptr)
		MetallicMap->BindToGPU(5);
	if (RoughnessMap != nullptr)
		RoughnessMap->BindToGPU(6);
}

const MeshMaterial::PerMaterialDesc & MeshMaterial::GetMaterialData() const
{
	return PerMaterialData;
}

void MeshMaterial::SetAmbient(const Color& InAmbient)
{
	PerMaterialData.Ambient = InAmbient;
}

void MeshMaterial::SetDiffuse(const Color& InDiffuse)
{
	PerMaterialData.Diffuse = InDiffuse;
}

void MeshMaterial::SetSpecular(const Color& InSpecular)
{
	PerMaterialData.Specular = InSpecular;
}

void MeshMaterial::SetMetallic(float InMetallic)
{
	PerMaterialData.Metallic = InMetallic;
}

void MeshMaterial::SetRoughness(float InRoughness)
{
	PerMaterialData.Glossiness = InRoughness;
}

void MeshMaterial::SetDiffuseMap(const wstring& InFilePath)
{
	DiffuseTex = new Texture(InFilePath);
}

void MeshMaterial::SetNormalMap(const wstring& InFilePath)
{
	NormalMap = new Texture(InFilePath);
}

void MeshMaterial::SetSpecularMap(const wstring& InFilePath)
{
	SpecularMap = new Texture(InFilePath);
}
#include "framework.h"
#include "Material.h"

Material::Material
(
	const Json::Value & InValue,
	const string & InMaterialName,
	const MaterialType InMaterialType
)
{
	if (InMaterialType != MaterialType::Mesh)
	{
		ASSERT(false, "Material Type Not Valid")
		return;
	}

	this->Name = InMaterialName;
	
	const Json::Value::Members Members = InValue.getMemberNames();
	// Read ShaderName
	this->ShaderName = InValue["ShaderName"].asString();
	this->Shader = ShaderManager::Get()->GetRenderShader(this->ShaderName);
	ASSERT(!!this->Shader, "Shader Not Valid");

	// Read Material Data
	this->MaterialInfo.Ambient =
		Helper::JsonToColor(InValue["Ambient"].asString());
	this->MaterialInfo.Diffuse =
		Helper::JsonToColor(InValue["Diffuse"].asString());
	this->MaterialInfo.Specular =
		Helper::JsonToColor(InValue["Specular"].asString());
	this->MaterialInfo.Metallic = stof(InValue["Metallic"].asString());
	this->MaterialInfo.Glossiness = stof(InValue["Roughness"].asString());
	this->bTransparent = static_cast<bool>(stoi(InValue["Transparent"].asString()));
	
	// Read Textures
	wstring TexturePath;
	TexturePath = String::ToWString(InValue["DiffuseMap"].asString());
	if (TexturePath.empty() == false)
	{
		this->DiffuseTex = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseDiffuseTexture;
	}
	TexturePath = String::ToWString(InValue["SpecularMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->SpecularMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseSpecularTexture;
	}
	TexturePath = String::ToWString(InValue["NormalMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->NormalMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseNormalTexture;
	}
	TexturePath = String::ToWString(InValue["ShininessMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->ShininessMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseShininessTexture;
	}
	TexturePath = String::ToWString(InValue["AlbedoMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->AlbedoMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseAlbedoTexture;
	}
	TexturePath = String::ToWString(InValue["MetallicMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->MetallicMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseMetallicTexture;
	}
	TexturePath = String::ToWString(InValue["RoughnessMap"].asString()); 
	if (TexturePath.empty() == false)
	{
		this->RoughnessMap = new Texture(TexturePath, true);
		this->MaterialInfo.TextureUsageFlag |= bUseRoughnessTexture;
	}
	
	CB_MaterialInfo = new ConstantBuffer(
		ShaderType::PixelShader,
		0,
		&MaterialInfo,
		sizeof(MaterialDesc),
		false
	);
}

Material::~Material()
{
	SAFE_DELETE(CB_MaterialInfo);
	SAFE_DELETE(DiffuseTex);
	SAFE_DELETE(NormalMap);
	SAFE_DELETE(SpecularMap);
}

void Material::Tick()
{
	if (bDirty == false)
		return;
	CB_MaterialInfo->UpdateData(&MaterialInfo, sizeof(MaterialDesc));
	bDirty = false;
}

void Material::BindToGpu(int RegisterIndex) const
{
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
	
	if (CB_MaterialInfo != nullptr)
		CB_MaterialInfo->BindToGPU(RegisterIndex);
}

void Material::SetAmbient(const Color& InAmbient)
{
	MaterialInfo.Ambient = InAmbient;
}

void Material::SetDiffuse(const Color& InDiffuse)
{
	MaterialInfo.Diffuse = InDiffuse;
}

void Material::SetSpecular(const Color& InSpecular)
{
	MaterialInfo.Specular = InSpecular;
}

void Material::SetMetallic(float InMetallic)
{
	MaterialInfo.Metallic = InMetallic;
}

void Material::SetRoughness(float InRoughness)
{
	MaterialInfo.Glossiness = InRoughness;
}

void Material::SetDiffuseMap(const wstring& InFilePath)
{
	DiffuseTex = new Texture(InFilePath);
}

void Material::SetNormalMap(const wstring& InFilePath)
{
	NormalMap = new Texture(InFilePath);
}

void Material::SetSpecularMap(const wstring& InFilePath)
{
	SpecularMap = new Texture(InFilePath);
}

const ConstantBuffer* Material::GetConstantBuffer() const
{
	return CB_MaterialInfo;
}

const string& Material::GetMaterialName() const
{
	return Name;
}

const string& Material::GetShaderName() const
{
	return ShaderName;
}

const RenderingShader* Material::GetRenderingShader() const
{
	return Shader;
}

const Material::MaterialDesc& Material::GetMaterialData() const
{
	return MaterialInfo;
}

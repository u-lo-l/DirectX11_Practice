#include "framework.h"
#include "Material.h"

Material::Material
(
	const Json::Value & InValue,
	const string & InMaterialName,
	const MaterialType InMaterialType
)
{
	if (InMaterialType != MaterialType::Mesh && InMaterialType != MaterialType::Terrain)
	{
		ASSERT(false, "Material Type Not Valid")
		return;
	}
	this->Name = InMaterialName;
	const Json::Value::Members Members = InValue.getMemberNames();
	this->ShaderName = InValue["ShaderName"].asString();
	this->Shader = ShaderManager::Get()->GetRenderShader(this->ShaderName);
	ASSERT(!!this->Shader, "Shader Not Valid");
}

Material::Material
(
	const string & InMaterialName,
	const MaterialType InMaterialType
)
{
	this->Name = InMaterialName;
	this->ShaderName = "Terrain";
	this->Shader = ShaderManager::Get()->GetRenderShader(this->ShaderName);
	ASSERT(!!this->Shader, "Shader Not Valid");
}

Material::~Material()
{
	SAFE_DELETE(CB_PerMaterial);
}

const ConstantBuffer* Material::GetConstantBuffer() const
{
	return CB_PerMaterial;
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
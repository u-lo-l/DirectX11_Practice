#include "framework.h"
#include "Material.h"

Material::Material
(
	const string & InMaterialName,
	const string & InShaderName,
	const MaterialType InMaterialType
)
{
	this->Name = InMaterialName;
	this->ShaderName = InShaderName;
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
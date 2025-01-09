// ReSharper disable All
#include "framework.h"
#include "Material.h"

Material::Material()
 : Drawer(nullptr), Textures{nullptr, }, SRVs{nullptr,}
{
}

Material::Material(Shader * InDrawer )
 : Drawer(InDrawer), Textures{nullptr,}, SRVs{nullptr,}
{
}

Material::Material( const wstring & InShaderFileName )
 : Drawer(new Shader(InShaderFileName)), Textures{nullptr,}, SRVs{nullptr,}
{
}

Material::~Material()
{
	for (int i = 0 ; i < ThisClass::MaxTextureCount ; i++)
	{
		SAFE_DELETE(Textures[i]);
	}
	SAFE_DELETE(Drawer);
}

void Material::Render()
{
	Drawer->AsSRV("MaterialMaps")->SetResourceArray((ID3D11ShaderResourceView **)&SRVs, 0, ThisClass::MaxTextureCount);
}

void Material::SetShader( const wstring & InShaderFileName )
{
	assert(InShaderFileName.length() > 0);
	Drawer = new Shader(InShaderFileName);
}

void Material::SetShader( Shader * InShader )
{
	Drawer = InShader;
}

Shader * Material::GetShader() const
{
	return Drawer;
}

void Material::SetDiffuseMap( const string & InFilePath )
{
	SetDiffuseMap(String::ToWString(InFilePath));
}

void Material::SetDiffuseMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Diffuse)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Diffuse)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Diffuse)] = Tex->GetSRV();
}

void Material::SetSpecularMap( const string & InFilePath )
{
	SetSpecularMap(String::ToWString(InFilePath));
}

void Material::SetSpecularMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Specular)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Specular)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Specular)] = Tex->GetSRV();
}

void Material::SetNormalMap( const string & InFilePath )
{
	SetNormalMap(String::ToWString(InFilePath));
}

void Material::SetNormalMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Normal)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Normal)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Normal)] = Tex->GetSRV();
}



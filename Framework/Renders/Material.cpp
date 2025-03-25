// ReSharper disable All
#include "framework.h"
#include "Material.h"

Material::Material()
 : Shader(nullptr), ColorData_CBuffer(nullptr), Textures{nullptr, }, SRVs{nullptr,}
{
	CreateBuffer();
}

Material::Material(HlslShader<VertexType> * InDrawer )
 : Shader(InDrawer), ColorData_CBuffer(nullptr), Textures{nullptr,}, SRVs{nullptr,}
{
	CreateBuffer();
}

Material::Material( const wstring & InShaderFileName )
 : Shader(new HlslShader<VertexType>(InShaderFileName)), ColorData_CBuffer(nullptr), Textures{nullptr,}, SRVs{nullptr,}
{
	CreateBuffer();
}

Material::~Material()
{
	for (int i = 0 ; i < ThisClass::MaxTextureCount ; i++)
	{
		SAFE_DELETE(Textures[i]);
	}
	SAFE_DELETE(Shader);
}

void Material::Tick()
{
	ColorData_CBuffer->UpdateData(&ColorData, sizeof(ThisClass::Colors));
}

void Material::Render()
{
	if (Shader == nullptr)
		return ;
	
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	ColorData_CBuffer->BindToGPU();
	DeviceContext->PSSetShaderResources(TextureSlot::PS_TextureMap, MaxTextureCount, SRVs);
}

void Material::CreateBuffer()
{
	if (!!ColorData_CBuffer)
		SAFE_DELETE(ColorData_CBuffer);
	ColorData_CBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		PS_Material,
		&ColorData,
		"Material.ColorData",
		sizeof(ThisClass::Colors),
		false
	);
}

void Material::SetShader( const wstring & InShaderFileName )
{
	assert(InShaderFileName.length() > 0);
	Shader = new HlslShader<VertexType>(InShaderFileName);
	SetShader(Shader);
}

void Material::SetShader( HlslShader<VertexType> * InShader )
{
	Shader = InShader;
	Shader->CreateRasterizerState_Solid();
	Shader->CreateSamplerState_Linear();
}

HlslShader<Material::VertexType> * Material::GetShader() const
{
	return Shader;
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



#pragma once
#include "Buffer/ConstantBuffer.h"
#include "Renders/Shader/HlslShader.hpp"

class String;

template<class TVertexType>
class Material
{
private:
	using VertexType = TVertexType;
	using ThisClass = Material;
	enum class TextureMapType : uint8_t  // NOLINT(performance-enum-size)
	{
		Diffuse = 0, Specular, Normal, Max
	};
	static constexpr UINT MaxTextureCount = static_cast<UINT>(TextureMapType::Max);
	
public:
	Material();
	explicit Material(HlslShader<VertexType> * InDrawer);
	explicit Material(const wstring & InShaderFileName);
	~Material();

	void Tick();
	void BindToGPU();

	void SetShader(const wstring & InShaderFileName, const D3D_SHADER_MACRO * ShaderMacro = nullptr);
	HlslShader<VertexType> * GetShader() const;
	HlslShader<VertexType> * GetShadowShader() const;
	void SetAmbient(const Color & InAmbient);
	void SetDiffuse(const Color & InDiffuse);
	void SetSpecular(const Color & InSpecular);
	void SetEmissive(const Color & InEmissive);
	void SetDiffuseMap(const string & InFilePath);
	void SetDiffuseMap(const wstring & InWFilePath);
	void SetSpecularMap(const string & InFilePath);
	void SetSpecularMap(const wstring & InWFilePath);
	void SetNormalMap(const string & InFilePath);
	void SetNormalMap(const wstring & InWFilePath);

private:
	void CreateBuffer();
	struct Colors
	{
		Color Ambient {0, 0, 0, 1};
		Color Diffuse {1, 1, 1, 1};
		Color Specular {0, 0, 0, 1};
		float RimWidth = 0.1f;
		float RimPower = 1.f;
		float Padding[2] = {0, 0};
	};
	
private:
	HlslShader<VertexType> * Shader = nullptr;
	HlslShader<VertexType> * ShadowShader = nullptr;
	Colors ColorData;
	ConstantBuffer * ColorData_CBuffer = nullptr;
	
	Texture * Textures[ThisClass::MaxTextureCount];
	ID3D11ShaderResourceView * SRVs[ThisClass::MaxTextureCount]; // Texture에서 해제됨.
};


/*==========================================================================================*/

template<class TVertexType>
Material<TVertexType>::Material()
 : Shader(nullptr), Textures{nullptr, }, SRVs{nullptr,}
{
	// ShadowShader = new HlslShader<VertexType>(L"Shadow/ShadowMap.hlsl");
}

template<class TVertexType>
Material<TVertexType>::Material(HlslShader<VertexType> * InDrawer )
 : Shader(InDrawer), Textures{nullptr,}, SRVs{nullptr,}
{
	// ShadowShader = new HlslShader<VertexType>(L"Shadow/ShadowMap.hlsl");
}

template<class TVertexType>
Material<TVertexType>::Material( const wstring & InShaderFileName )
 : Shader(new HlslShader<VertexType>(InShaderFileName)), Textures{nullptr,}, SRVs{nullptr,}
{
	// ShadowShader = new HlslShader<VertexType>(L"Shadow/ShadowMap.hlsl");
}

template<class TVertexType>
Material<TVertexType>::~Material()
{
	for (int i = 0 ; i < ThisClass::MaxTextureCount ; i++)
	{
		SAFE_DELETE(Textures[i]);
	}
	SAFE_DELETE(Shader);
	SAFE_DELETE(ShadowShader);
}

template<class TVertexType>
void Material<TVertexType>::Tick()
{
	ColorData_CBuffer->UpdateData(&ColorData, sizeof(typename ThisClass::Colors));
}

template<class TVertexType>
void Material<TVertexType>::BindToGPU()
{
	if (Shader == nullptr)
		return ;
	
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	ColorData_CBuffer->BindToGPU();
	DeviceContext->PSSetShaderResources(TextureSlot::PS_TextureMap, MaxTextureCount, SRVs);
}

template<class TVertexType>
void Material<TVertexType>::CreateBuffer()
{
	if (!!ColorData_CBuffer)
		SAFE_DELETE(ColorData_CBuffer);
	
	ColorData_CBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		PS_Material,
		&ColorData,
		"Material.ColorData",
		sizeof(typename ThisClass::Colors),
		true
	);
}

template<class TVertexType>
void Material<TVertexType>::SetShader( const wstring & InShaderFileName, const D3D_SHADER_MACRO * ShaderMacro)
{
	assert(InShaderFileName.length() > 0);
	UINT TargetShaderFlag = static_cast<UINT>(ShaderType::VertexShader) | static_cast<UINT>(ShaderType::PixelShader);
	
	Shader = new HlslShader<VertexType>(
		InShaderFileName,
		TargetShaderFlag,
		"VSMain",
		"PSMain",
		"",
		ShaderMacro
	);
	Shader->CreateRasterizerState_Solid();
	Shader->CreateSamplerState_Linear();
	Shader->CreateSamplerState_Anisotropic();
	Shader->CreateBlendState_NoBlend();

	
	ShadowShader = new HlslShader<VertexType>(
		InShaderFileName,
		TargetShaderFlag,
		"VSShadow",
		"PSShadow",
		"",
		ShaderMacro
	);
	ShadowShader->CreateRasterizerState_Solid();
	ShadowShader->CreateSamplerState_ShadowSampler();
	ShadowShader->CreateBlendState_NoBlend();

	CreateBuffer();
}

template<class TVertexType>
HlslShader<TVertexType> * Material<TVertexType>::GetShader() const
{
	return Shader;
}

template <class TVertexType>
HlslShader<TVertexType> * Material<TVertexType>::GetShadowShader() const
{
	return ShadowShader;
}

template <class TVertexType>
void Material<TVertexType>::SetAmbient(const Color& InAmbient)
{
	ColorData.Ambient = InAmbient;
}

template <class TVertexType>
void Material<TVertexType>::SetDiffuse(const Color& InDiffuse)
{
	ColorData.Diffuse = InDiffuse;
}

template <class TVertexType>
void Material<TVertexType>::SetSpecular(const Color& InSpecular)
{
	ColorData.Specular = InSpecular;
}

template <class TVertexType>
void Material<TVertexType>::SetEmissive(const Color& InEmissive)
{
	ColorData.RimWidth = InEmissive[0];
	ColorData.RimPower = InEmissive[1] + 1.f;
}

template<class TVertexType>
void Material<TVertexType>::SetDiffuseMap( const string & InFilePath )
{
	SetDiffuseMap(String::ToWString(InFilePath));
}

template<class TVertexType>
void Material<TVertexType>::SetDiffuseMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Diffuse)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Diffuse)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Diffuse)] = Tex->GetSRV();
}

template<class TVertexType>
void Material<TVertexType>::SetSpecularMap( const string & InFilePath )
{
	SetSpecularMap(String::ToWString(InFilePath));
}

template<class TVertexType>
void Material<TVertexType>::SetSpecularMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Specular)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Specular)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Specular)] = Tex->GetSRV();
}

template<class TVertexType>
void Material<TVertexType>::SetNormalMap( const string & InFilePath )
{
	SetNormalMap(String::ToWString(InFilePath));
}

template<class TVertexType>
void Material<TVertexType>::SetNormalMap( const wstring & InWFilePath )
{
	SAFE_DELETE(Textures[(int)(TextureMapType::Normal)]);
	Texture * const Tex = new Texture(InWFilePath);
	Textures[static_cast<int>(TextureMapType::Normal)] = Tex;
	SRVs[static_cast<int>(TextureMapType::Normal)] = Tex->GetSRV();
}


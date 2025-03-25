#pragma once
#include "Renders/Shader/HlslShader.hpp"

class ConstantBuffer;

class Material
{
private:
	using VertexType = ModelVertex;
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
	void Render();

	void SetShader(const wstring & InShaderFileName);
	void SetShader(HlslShader<VertexType> * InShader);
	HlslShader<VertexType> * GetShader() const;
	void SetAmbient(const Color & InAmbient) { ColorData.Ambient = InAmbient; }
	void SetDiffuse(const Color & InDiffuse) { ColorData.Diffuse = InDiffuse; }
	void SetSpecular(const Color & InSpecular) { ColorData.Specular = InSpecular; }
	void SetEmissive(const Color & InEmissive) { ColorData.Emissive = InEmissive; }
	void SetDiffuseMap(const string & InFilePath);
	void SetDiffuseMap(const wstring & InWFilePath);
	void SetSpecularMap(const string & InFilePath);
	void SetSpecularMap(const wstring & InWFilePath);
	void SetNormalMap(const string & InFilePath);
	void SetNormalMap(const wstring & InWFilePath);

private:
	struct Colors
	{
		Color Ambient {0, 0, 0, 1};
		Color Diffuse {1, 1, 1, 1};
		Color Specular {0, 0, 0, 1};
		Color Emissive {0, 0, 0, 1};
	};
	
private:
	HlslShader<VertexType> * Shader;
	
	Colors ColorData;

	ConstantBuffer * ColorData_CBuffer = nullptr;
	// IECB_t * ECB_Color = nullptr; // sCBuffer : in course
	
	Texture * Textures[ThisClass::MaxTextureCount];
	ID3D11ShaderResourceView * SRVs[ThisClass::MaxTextureCount];
	// IESRV_t * ESRV_TextureMap = nullptr; // sSRVs : in course
};


#pragma once
#include "Material.h"

class MeshMaterial final : public Material
{
private:
	static constexpr UINT bUseNormalTexture = 1 << 0;
	static constexpr UINT bUseDiffuseTexture = 1 << 1;
	static constexpr UINT bUseSpecularTexture = 1 << 2;
	static constexpr UINT bUseShininessTexture = 1 << 3;
	static constexpr UINT bUseAlbedoTexture = 1 << 3;
	static constexpr UINT bUseMetallicTexture = 1 << 4;
	static constexpr UINT bUseRoughnessTexture = 1 << 5;
	enum class TextureMapType : uint8_t  // NOLINT(performance-enum-size)
	{
		Normal = 0,
		Diffuse,
		Specular,
		Shininess,
		Metallic,
		Roughness,
		Max
	};
	struct PerMaterialDesc
	{
		Color Ambient = {0.1f,0.1f,0.1f,0};
		Color Diffuse = {1.0f,1.0f, 1.0f,0};
		Color Specular = {0,0,0,0};
		float Metallic = 0.0f;
		float Glossiness = 1.f;
		UINT TextureUsageFlag = 0;
		float Padding;
	};
public:
	MeshMaterial(const Json::Value & InValue, const string& InMaterialName, const string & InShaderName);
	virtual ~MeshMaterial() override;
	virtual void Tick() override;
	virtual void BindToGpu(int RegisterIndex) const override;
	
	[[nodiscard]] const PerMaterialDesc & GetMaterialData() const;
	[[nodiscard]] bool IsTransparent() const { return bTransparent; }
	
	void SetAmbient(const Color& InAmbient);
	void SetDiffuse(const Color& InDiffuse);
	void SetSpecular(const Color& InSpecular);
	void SetMetallic(float InMetallic);
	void SetRoughness(float InRoughness);
	void SetDiffuseMap(const wstring & InFilePath);
	void SetNormalMap(const wstring & InFilePath);
	void SetSpecularMap(const wstring & InFilePath);
private:
	PerMaterialDesc PerMaterialData;
	bool bTransparent = false;
	
	Texture * NormalMap = nullptr;	// 0
	Texture * DiffuseTex = nullptr; // 1
	Texture * SpecularMap = nullptr; // 2
	Texture * ShininessMap = nullptr; // 3

	Texture * AlbedoMap = nullptr; // 4
	Texture * MetallicMap = nullptr; // 5
	Texture * RoughnessMap = nullptr; // 6
};

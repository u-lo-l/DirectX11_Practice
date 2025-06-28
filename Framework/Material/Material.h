#pragma once

class RenderingShader;

class Material
{
public:
	virtual void Tick() = 0;
	virtual void BindToGpu(int RegisterIndex) const = 0;

	[[nodiscard]] const ConstantBuffer * GetConstantBuffer() const;
	[[nodiscard]] const string & GetMaterialName() const;
	[[nodiscard]] const string & GetShaderName() const;
	[[nodiscard]] const RenderingShader * GetRenderingShader() const;
	
protected:
	string Name;
	string ShaderName;
	RenderingShader * Shader;
	bool bDirty = false;
	ConstantBuffer * CB_PerMaterial = nullptr;
protected:
	static constexpr UINT bUseNormalTexture = 1 << 0;
	static constexpr UINT bUseDiffuseTexture = 1 << 1;
	static constexpr UINT bUseSpecularTexture = 1 << 2;
	static constexpr UINT bUseShininessTexture = 1 << 3;
	static constexpr UINT bUseAlbedoTexture = 1 << 3;
	static constexpr UINT bUseMetallicTexture = 1 << 4;
	static constexpr UINT bUseRoughnessTexture = 1 << 5;
	enum class MaterialType : uint8_t
	{
		Line = 0,
		Sprite,
		Mesh,
		Terrain,
		Foliage,
		Ocean,
		Particle,
	};
	Material(const Json::Value & InValue, const string& InMaterialName, MaterialType InMaterialType);
	Material(const string& InMaterialName, MaterialType InMaterialType);
	~Material();
};

#pragma once

class RenderingShader;

class Material
{
private:
	static constexpr UINT bUseNormalTexture = 1 << 0;
	static constexpr UINT bUseDiffuseTexture = 1 << 1;
	static constexpr UINT bUseSpecularTexture = 1 << 2;
	static constexpr UINT bUseShininessTexture = 1 << 3;
	static constexpr UINT bUseAlbedoTexture = 1 << 3;
	static constexpr UINT bUseMetallicTexture = 1 << 4;
	static constexpr UINT bUseRoughnessTexture = 1 << 5;
public:
	static constexpr UINT TexturePerSurface = 1;
	enum MaterialType
	{
		Line = 0,
		Sprite,
		Mesh,
		Terrain,
		Foliage,
		Ocean,
		Particle,
	};
private:
	enum class TextureMapType : uint8_t  // NOLINT(performance-enum-size)
	{
		Diffuse = 0, Specular, Normal, Max
	};
	struct MaterialDesc
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
	Material(const Json::Value & InValue, const string& InMaterialName, MaterialType InMaterialType);
	~Material();
	void Tick();
	void BindToGpu(int RegisterIndex) const;
	
	void SetAmbient(const Color& InAmbient);
	void SetDiffuse(const Color& InDiffuse);
	void SetSpecular(const Color& InSpecular);
	void SetMetallic(float InMetallic);
	void SetRoughness(float InRoughness);
	void SetDiffuseMap(const wstring & InFilePath);
	void SetNormalMap(const wstring & InFilePath);
	void SetSpecularMap(const wstring & InFilePath);
	
	const ConstantBuffer * GetConstantBuffer() const;
	const string & GetMaterialName() const;
	const string & GetShaderName() const;
	const RenderingShader * GetRenderingShader() const;
	const MaterialDesc & GetMaterialData() const;
	bool IsTransparent() const { return bTransparent; };
private:
	string Name;
	string ShaderName;
	RenderingShader * Shader;
	MaterialDesc MaterialInfo;
	bool bTransparent = false;
	
	bool bDirty = false;
	ConstantBuffer * CB_MaterialInfo = nullptr;
	Texture * NormalMap = nullptr;	// 0
	Texture * DiffuseTex = nullptr; // 1
	Texture * SpecularMap = nullptr; // 2
	Texture * ShininessMap = nullptr; // 3

	Texture * AlbedoMap = nullptr; // 4
	Texture * MetallicMap = nullptr; // 5
	Texture * RoughnessMap = nullptr; // 6

};

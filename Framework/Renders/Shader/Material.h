#pragma once

class RenderingShader;

class Material
{
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
		float Roughness = 1.f;
		float Padding[2];
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
	Texture * DiffuseTex = nullptr;
	Texture * NormalMap = nullptr;
	Texture * SpecularMap = nullptr;
};

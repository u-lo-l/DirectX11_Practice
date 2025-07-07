#pragma once

class OceanMaterial final : public Material
{
public:
	struct MaterialDesc
	{
		const Texture * HeightMap = nullptr;
		UINT DisplacementMapSize = 0;
		float DisplacementMapTiling = 0.f;
		float NoiseTiling = 0.f;
		string Name;
		string ShaderName;
	};
	explicit OceanMaterial(const MaterialDesc & Desc);
	virtual ~OceanMaterial() override;
	virtual void BindToGpu(int RegisterIndex) const override;
	virtual void Tick() override;
	float GetDisplacementMapTiling() const { return PerMaterialData.TilingData.DisplacementMapTiling; }
	float GetNoiseTiling() const { return PerMaterialData.TilingData.NoiseTiling; }
	
	RWTexture2DArray * GetDisplacementMaps() const { return DisplacementMap; }
	RWTexture2DArray * GetNormalMaps() const { return NormalMap; }
	RWTexture2DArray * GetFoamGridMaps() const { return FoamGrid; }
private:
	struct TilingDesc
	{
		float DisplacementMapTiling = 1.f;
		float NoiseTiling = 1.f;
		Vector2D TextureSize;
	};
	struct ShadingDesc
	{
		Vector AirBubbleColor = { 0.35f, 0.51f, 0.69f };
		float AmbientIntensity = 0.51f;
		Vector SpecularColor = {0.87f, 0.08f, 0.08f };
		float SpecularIntensity = 1.f;
		Vector WaterScatteringColor = { 0.55891776f, 0.754717f, 0.754717f };
		float AirBubbleDensity = 0.45f;
		
		float SurfaceIntensity1 = 0.05f;
		float SurfaceIntensity2 = 0.72f;
		float ReflectionIntensity = 0.41f;
		float Shininess = 50.f;
	};
	struct CascadeDesc
	{
		float Weight;
		float UVScaler;
		float DisplacementScaler;
		float FoamScaler;
	};
	struct PerMaterialDesc
	{
		TilingDesc TilingData;
		CascadeDesc CascadeData[3];
		ShadingDesc ShadingData;
	} PerMaterialData;
	string Name;
	string ShaderName;
	constexpr static int CascadeCount = 3;
	RWTexture2DArray * DisplacementMap = nullptr;
	RWTexture2DArray * NormalMap = nullptr;
	RWTexture2DArray * FoamGrid = nullptr;
	
	Texture * PerlinNoise = nullptr;
	
};

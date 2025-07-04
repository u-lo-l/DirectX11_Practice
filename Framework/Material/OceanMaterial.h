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
	float GetDisplacementMapTiling() const { return PerMaterialData.DisplacementMapTiling; }
	float GetNoiseTiling() const { return PerMaterialData.NoiseTiling; }
	
	RWTexture2DArray * GetDisplacementMaps() const { return DisplacementMap; }
	RWTexture2DArray * GetNormalMaps() const { return NormalMap; }
	RWTexture2DArray * GetFoamGridMaps() const { return FoamGrid; }
private:
	struct PerMaterialDesc
	{
		float DisplacementMapTiling = 1.f;
		float NoiseTiling = 1.f;
		Vector2D TextureSize;
	} PerMaterialData;
	string Name;
	string ShaderName;
	constexpr static int CascadeCount = 3;
	RWTexture2DArray * DisplacementMap = nullptr;
	RWTexture2DArray * NormalMap = nullptr;
	RWTexture2DArray * FoamGrid = nullptr;
};

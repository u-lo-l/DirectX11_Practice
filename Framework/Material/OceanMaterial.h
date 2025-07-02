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
	
	RWTexture2D * GetDisplacementMap() const { return DisplacementMap; }
	RWTexture2D * GetNormalMap() const { return NormalMap; }
	RWTexture2D * GetFoamGridMap() const { return FoamGrid; }
private:
	struct PerMaterialDesc
	{
		float DisplacementMapTiling = 1.f;
		float NoiseTiling = 1.f;
		Vector2D TextureSize;
	} PerMaterialData;
	string Name;
	string ShaderName;
	RWTexture2D * DisplacementMap = nullptr;
	RWTexture2D * NormalMap = nullptr;
	RWTexture2D * FoamGrid = nullptr;
};

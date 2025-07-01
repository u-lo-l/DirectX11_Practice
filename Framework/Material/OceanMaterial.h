#pragma once

class OceanMaterial final : public Material
{
public:
	struct MaterialDesc
	{
		const Texture * HeightMap = nullptr;
		float DisplacementMapTiling = 1.f;
		float NoiseTiling = 1.f;
		string Name = "OceanMat";
		string ShaderName = "Ocean";
	};
	explicit OceanMaterial(const MaterialDesc & Desc);
	virtual ~OceanMaterial() override;
	virtual void BindToGpu(int RegisterIndex) const override;
	virtual void Tick() override;

private:
	struct PerMaterialDesc
	{
		float DisplacementMapTiling = 1.f;
		float NoiseTiling = 1.f;
		float WaterRefractionIndex = 1.33f; // 굴절률
		float WaterR0 = 0.02f;              // 수직 입사 반사 계수
	} PerMaterialData;
};

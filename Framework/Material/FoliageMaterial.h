#pragma once

class FoliageMaterial final : public Material
{
public:
	struct MaterialDesc
	{
		string Name = "Mat_Foliage";
		string ShaderName = "Foliage";
		wstring DensityMapName = L"Terrain/Foliage/FoliageDensity.png";
		vector<wstring> FoliageTextureNames;
	};
	explicit FoliageMaterial(const MaterialDesc& InDesc);
	virtual ~FoliageMaterial() override;
	virtual void BindToGpu(int RegisterIndex) const override;
	virtual void Tick() override;
private:
	Texture * FoliageDensity = nullptr;
	TextureArray * FoliageTextures = nullptr;
};

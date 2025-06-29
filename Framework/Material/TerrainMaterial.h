#pragma once

class TerrainMaterial final : public Material
{
public:
	struct MaterialDesc
	{
		const Texture * HeightMap = nullptr;
		wstring MacroVariation = L"Terrain/T_MacroVariation.png";
		wstring PerlinNoise = L"Terrain/T_Perlin_Noise.png";
		vector<wstring> DiffuseTextures {L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"};
		vector<wstring> NormalTextures {L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"};
	};
	explicit TerrainMaterial(const MaterialDesc & Desc);
	virtual ~TerrainMaterial();
	virtual void BindToGpu(int RegisterIndex) const override;
	virtual void Tick() override;
private:
	struct PerMaterialDesc
	{
		float NearSize = 1.0f;
		float FarSize = 0.1f;
		float StartOffset = -1000.f;
		float Range = 5000.f;
		// Perlin Noise
		float NoiseAmount = 1.f;
		float NoisePower  = 1.f;
		// Slope Based
		float SlopBias = 60;
		float SlopSharpness = 3;
		// Altitude Base
		float LowHeight = 5;
		float HighHeight = 15;
		float HeightSharpness = 1;
		// Padding
		float Padding;
	} PerMaterialData;
	Texture * PerlinNoise = nullptr;
	Texture * MacroVariation = nullptr;
	TextureArray * DiffuseMaps = nullptr;
	TextureArray * NormalMaps = nullptr;
};

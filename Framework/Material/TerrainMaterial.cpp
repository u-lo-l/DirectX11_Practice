#include "framework.h"
#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(const MaterialDesc& Desc)
	: Material("TerrainMat", "Terrain", MaterialType::Terrain)
{
	PerlinNoise = new Texture(Desc.PerlinNoise, true);
	MacroVariation = new Texture(Desc.MacroVariation, true);

	NormalMaps = new TextureArray(Desc.NormalTextures, 1024, 1024, 5);
	DiffuseMaps = new TextureArray(Desc.DiffuseTextures, 1024, 1024, 5);
	bDirty = true;

	CB_PerMaterial = new ConstantBuffer(
		ShaderType::PixelShader,
		0,
		&PerMaterialData,
		sizeof(PerMaterialDesc),
		false
	);
}

TerrainMaterial::~TerrainMaterial()
{
	SAFE_DELETE(MacroVariation);
	SAFE_DELETE(PerlinNoise);
	SAFE_DELETE(DiffuseMaps);
	SAFE_DELETE(NormalMaps);
}

void TerrainMaterial::BindToGpu(int RegisterIndex) const
{
	CHECK(!!MacroVariation);
	CHECK(!!PerlinNoise);
	CHECK(!!DiffuseMaps);
	CHECK(!!NormalMaps);
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(RegisterIndex);
	MacroVariation->BindToGPU(3, ShaderType::PixelShader);
	PerlinNoise->BindToGPU(4, ShaderType::PixelShader);
	DiffuseMaps->BindToGPU(5);
	NormalMaps->BindToGPU(6);
}

void TerrainMaterial::Tick()
{
	if (bDirty == false)
		return;
	CB_PerMaterial->UpdateData(&PerMaterialData, sizeof(PerMaterialData));
	bDirty = false;
}

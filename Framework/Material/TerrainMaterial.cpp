#include "framework.h"
#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(const MaterialDesc& Desc)
	: Material("TerrainMat", MaterialType::Terrain)
{
	PerlinNoise = new Texture(Desc.PerlinNoise, true);
	MacroVariation = new Texture(Desc.MacroVariation, true);

	DiffuseMaps = new TextureArray(Desc.DiffuseTextures, 1024, 1024, 5);
	NormalMaps = new TextureArray(Desc.NormalTextures, 1024, 1024, 5);
	bDirty = true;

	ComputeShader * NormalMapCreator = ShaderManager::Get()->GetComputeShader("NormalMapCreator");
	ASSERT(!!NormalMapCreator, "NormalMapCreate Not Found");
	const UINT TextureWidth  = Desc.HeightMap->GetWidth();
	const UINT TextureHeight = Desc.HeightMap->GetHeight();
	NormalMap = new RWTexture2D(TextureWidth, TextureHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	TangentMap= new RWTexture2D(TextureWidth, TextureHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	struct CB_Desc
	{
		float HeightScaler = 1.f;
		UINT TextureWidth;
		UINT TextureHeight;
		float Padding;
	} CB_Data;
	CB_Data.HeightScaler = 2048.f;
	CB_Data.TextureWidth = TextureWidth;
	CB_Data.TextureHeight = TextureHeight;
	ConstantBuffer CB = ConstantBuffer(
		ShaderType::ComputeShader,0,
		&CB_Data,sizeof(CB_Data),true
	);
	CB.BindToGPU(ShaderType::ComputeShader, 0);
	Desc.HeightMap->BindToGPU(0, ShaderType::ComputeShader);
	NormalMap->BindToGPUAsUAV(0);
	TangentMap->BindToGPUAsUAV(1);
	UINT X, Y, Z;
	NormalMapCreator->GetThreadDim(X, Y, Z);
	NormalMapCreator->Dispatch(TextureWidth / X, TextureHeight / Y, 1);
	CB_PerMaterial = new ConstantBuffer(
		ShaderType::PixelShader,
		0,
		&PerMaterialData,
		sizeof(PerMaterialDesc),
		false
	);
}

void TerrainMaterial::BindToGpu(int RegisterIndex) const
{
	CHECK(!!NormalMap);
	CHECK(!!TangentMap);
	CHECK(!!MacroVariation);
	CHECK(!!PerlinNoise);
	CHECK(!!DiffuseMaps);
	CHECK(!!NormalMaps);
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(RegisterIndex);

	NormalMap->BindToGPUAsSRV(1, ShaderType::PixelShader);
	TangentMap->BindToGPUAsSRV(2, ShaderType::PixelShader);
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

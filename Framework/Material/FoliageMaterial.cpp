#include "framework.h"
#include "FoliageMaterial.h"

FoliageMaterial::FoliageMaterial(const MaterialDesc& InDesc)
	: Material(InDesc.Name, InDesc.ShaderName, MaterialType::Foliage)
{
	FoliageDensity = new Texture(InDesc.DensityMapName, true);
	FoliageTextures = new TextureArray(InDesc.FoliageTextureNames);
	CB_PerMaterial = nullptr;
}

FoliageMaterial::~FoliageMaterial()
{
	SAFE_DELETE(CB_PerMaterial);
}

void FoliageMaterial::Tick()
{
}

void FoliageMaterial::BindToGpu(int RegisterIndex) const
{
	CHECK(!!FoliageDensity);
	CHECK(!!FoliageTextures);
	
	FoliageDensity->BindToGPU(2, ShaderType::GeometryShader);
	FoliageTextures->BindToGPU(3);
}

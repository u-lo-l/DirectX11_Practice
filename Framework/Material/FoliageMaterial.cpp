#include "framework.h"
#include "FoliageMaterial.h"

FoliageMaterial::FoliageMaterial(const MaterialDesc& InDesc)
	: Material(InDesc.Name, InDesc.ShaderName, MaterialType::Foliage)
{
	ASSERT(InDesc.ShaderName.empty() == false, "ShaderName Not Assigned");
	ASSERT(InDesc.DensityMapName.empty() == false, "DensityMapName Not Assigned");
	ASSERT(InDesc.FoliageTextureNames.empty() == false, "FoliageTextureNames Not Assigned");
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

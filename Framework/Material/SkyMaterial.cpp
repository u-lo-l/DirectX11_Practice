#include "framework.h"
#include "SkyMaterial.h"

SkyMaterial::SkyMaterial(const MaterialDesc& Desc)
: Material(Desc.Name, Desc.ShaderName, MaterialType::SkyBox)
{
	ASSERT(Desc.ShaderName.empty() == false, "ShaderName Not Assigned");
	ASSERT(Desc.SkyTextureName.empty() == false, "SkyTexture Not Assigned");
	Skybox = new Texture(Desc.SkyTextureName, true);
}

SkyMaterial::~SkyMaterial()
{
	SAFE_DELETE(Skybox);
}

void SkyMaterial::Tick()
{
}

void SkyMaterial::BindToGpu(int RegisterIndex) const
{
	Skybox->BindToGPU(10, ShaderType::PixelShader);
}

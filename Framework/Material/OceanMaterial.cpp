#include "framework.h"
#include "OceanMaterial.h"

OceanMaterial::OceanMaterial(const MaterialDesc& Desc)
: Material(Desc.Name, Desc.ShaderName, MaterialType::Ocean)
{
	ASSERT(Desc.DisplacementMapSize > 0, "Texture Size Not Assigned");
	ASSERT(Desc.DisplacementMapTiling > 0, "Texture Tiling Not Assigned");
	ASSERT(Desc.NoiseTiling > 0, "Noise Tiling Not Assigned");
	ASSERT(Desc.ShaderName.empty() == false, "ShaderName Not Assigned");
	bDirty = true;
	
	DisplacementMap = new RWTexture2D(
		Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	NormalMap = new RWTexture2D(
		Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	FoamGrid = new RWTexture2D(
		Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	Name = Desc.Name;
	ShaderName = Desc.ShaderName;
	PerMaterialData.DisplacementMapTiling = Desc.DisplacementMapTiling;
	PerMaterialData.NoiseTiling = Desc.NoiseTiling;
	PerMaterialData.TextureSize = DisplacementMap->GetSize();

	CB_PerMaterial = new ConstantBuffer(
		ShaderType::DP,
		0,
		&PerMaterialData,
		sizeof(PerMaterialDesc),
		false
	);
}

OceanMaterial::~OceanMaterial()
{
	// SAFE_DELETE(PerlinNoise);
	SAFE_DELETE(CB_PerMaterial);
	SAFE_DELETE(DisplacementMap);
	SAFE_DELETE(NormalMap);
	SAFE_DELETE(FoamGrid);
}

void OceanMaterial::BindToGpu(int RegisterIndex) const
{
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(ShaderType::DP, RegisterIndex);
	// PerlinNoise->BindToGPU(5, ShaderType::PixelShader);
	CHECK(!!DisplacementMap);
	DisplacementMap->BindToGPUAsSRV(0, ShaderType::VDP);
	NormalMap->BindToGPUAsSRV(1, ShaderType::VDP);
	FoamGrid->BindToGPUAsSRV(2, ShaderType::VDP);
}

void OceanMaterial::Tick()
{
	// ImGui::SliderFloat("Tiling", &PerMaterialData.DisplacementMapTiling, 1.f, 100.f, "%.0f");
	if (bDirty == false)
		return;
	CB_PerMaterial->UpdateData(&PerMaterialData, sizeof(PerMaterialData));
	bDirty = false;
}


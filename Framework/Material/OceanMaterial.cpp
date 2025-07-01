#include "framework.h"
#include "OceanMaterial.h"

OceanMaterial::OceanMaterial(const MaterialDesc& Desc)
: Material(Desc.Name, Desc.ShaderName, MaterialType::Ocean)
{
	bDirty = true;
	// PerMaterialData.DisplacementMapTiling = Desc.Name;
	// PerMaterialData.DisplacementMapTiling = Desc.ShaderName;
	PerMaterialData.DisplacementMapTiling = Desc.DisplacementMapTiling;
	// PerMaterialData.DisplacementMapTiling = Desc.NoiseTiling;
	
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
}

void OceanMaterial::BindToGpu(int RegisterIndex) const
{
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(ShaderType::DP, RegisterIndex);
	// PerlinNoise->BindToGPU(5, ShaderType::PixelShader);
}

void OceanMaterial::Tick()
{
	ImGui::SliderFloat("Tiling", &PerMaterialData.DisplacementMapTiling, 1.f, 100.f, "%.0f");
	CB_PerMaterial->UpdateData(&PerMaterialData, sizeof(PerMaterialData));
	// if (bDirty == false)
	// 	return;
	// bDirty = false;
}


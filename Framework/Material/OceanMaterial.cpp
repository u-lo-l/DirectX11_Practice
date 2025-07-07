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
	
	DisplacementMap = new RWTexture2DArray(
		CascadeCount, Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	NormalMap = new RWTexture2DArray(
		CascadeCount, Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	FoamGrid = new RWTexture2DArray(
		CascadeCount,Desc.DisplacementMapSize, Desc.DisplacementMapSize,
		DXGI_FORMAT_R32G32_FLOAT
	);
	PerlinNoise = new Texture(L"Terrain/T_Perlin_Noise.png", true);
	Name = Desc.Name;
	ShaderName = Desc.ShaderName;
	PerMaterialData.TilingData.DisplacementMapTiling = Desc.DisplacementMapTiling;
	PerMaterialData.TilingData.NoiseTiling = Desc.NoiseTiling;
	PerMaterialData.TilingData.TextureSize = DisplacementMap->GetSize();

	PerMaterialData.CascadeData[0] = {1.f, 1.f, 1.f, 1.f};
	PerMaterialData.CascadeData[1] = {1.f, 1.f, 1.f, 1.f};
	PerMaterialData.CascadeData[2] = {1.f, 1.f, 1.f, 1.f};

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
	SAFE_DELETE(CB_PerMaterial);
	SAFE_DELETE(DisplacementMap);
	SAFE_DELETE(NormalMap);
	SAFE_DELETE(FoamGrid);
}

void OceanMaterial::BindToGpu(int RegisterIndex) const
{
	if (CB_PerMaterial != nullptr)
		CB_PerMaterial->BindToGPU(ShaderType::DP, RegisterIndex);
	CHECK(!!DisplacementMap);
	DisplacementMap->BindToGPUAsSRV(0, ShaderType::DomainShader);
	NormalMap->BindToGPUAsSRV(1, ShaderType::PixelShader);
	FoamGrid->BindToGPUAsSRV(2, ShaderType::DP);
	// PerlinNoise->BindToGPU(3, ShaderType::DP);
}

void OceanMaterial::Tick()
{
	// ImGui::SliderFloat("Tiling", &PerMaterialData.DisplacementMapTiling, 1.f, 100.f, "%.0f");
	// if (bDirty == false)
	// 	return;
	// bDirty = false;
	ImGui::Begin("Ocean Material");
	ImGui::BeginChild("Shading");
	ImGui::ColorEdit3("AirBubbleColor", PerMaterialData.ShadingData.AirBubbleColor, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("SpecularColor", PerMaterialData.ShadingData.SpecularColor, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("WaterScatteringColor", PerMaterialData.ShadingData.WaterScatteringColor, ImGuiColorEditFlags_Float);
	ImGui::SliderFloat("AirBubbleDensity", &PerMaterialData.ShadingData.AirBubbleDensity, 0, 1);
	ImGui::SliderFloat("AmbientIntensity", &PerMaterialData.ShadingData.AmbientIntensity, 0, 1);
	ImGui::SliderFloat("SurfaceIntensity1", &PerMaterialData.ShadingData.SurfaceIntensity1, 0, 1);

	ImGui::SliderFloat("SurfaceIntensity2", &PerMaterialData.ShadingData.SurfaceIntensity2, 0, 1);
	ImGui::SliderFloat("ReflectionIntensity", &PerMaterialData.ShadingData.ReflectionIntensity, 0, 1);
	ImGui::SliderFloat("SpecularIntensity", &PerMaterialData.ShadingData.SpecularIntensity, 0, 1);
	ImGui::SliderFloat("Shininess", &PerMaterialData.ShadingData.Shininess, 1, 100);
	ImGui::EndChild();
	ImGui::BeginChild("Cascade");
	ImGui::Text("Cascade1");
	ImGui::SliderFloat(String::Format("Weight_1").c_str(), &PerMaterialData.CascadeData[0].Weight, 0, 2);
	ImGui::SliderFloat(String::Format("UVScaler_1").c_str(), &PerMaterialData.CascadeData[0].UVScaler, 0, 2);
	ImGui::SliderFloat(String::Format("DisplacementScaler_1").c_str(), &PerMaterialData.CascadeData[0].DisplacementScaler, 0, 2);
	ImGui::SliderFloat(String::Format("FoamScaler_1").c_str(), &PerMaterialData.CascadeData[0].FoamScaler, 0, 2);
	ImGui::Text("Cascade2");
	ImGui::SliderFloat(String::Format("Weight_2").c_str(), &PerMaterialData.CascadeData[1].Weight, 0, 2);
	ImGui::SliderFloat(String::Format("UVScaler_2").c_str(), &PerMaterialData.CascadeData[1].UVScaler, 0, 2);
	ImGui::SliderFloat(String::Format("DisplacementScaler_2").c_str(), &PerMaterialData.CascadeData[1].DisplacementScaler, 0, 2);
	ImGui::SliderFloat(String::Format("FoamScaler_2").c_str(), &PerMaterialData.CascadeData[1].FoamScaler, 0, 2);
	ImGui::Text("Cascade3");
	ImGui::SliderFloat(String::Format("Weight_3").c_str(), &PerMaterialData.CascadeData[2].Weight, 0, 2);
	ImGui::SliderFloat(String::Format("UVScaler_3").c_str(), &PerMaterialData.CascadeData[2].UVScaler, 0, 2);
	ImGui::SliderFloat(String::Format("DisplacementScaler_3").c_str(), &PerMaterialData.CascadeData[2].DisplacementScaler, 0, 2);
	ImGui::SliderFloat(String::Format("FoamScaler_3").c_str(), &PerMaterialData.CascadeData[2].FoamScaler, 0, 2);
	ImGui::EndChild();
	ImGui::End();
	CB_PerMaterial->UpdateData(&PerMaterialData, sizeof(PerMaterialDesc));
}


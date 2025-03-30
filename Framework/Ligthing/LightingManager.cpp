#include "framework.h"
#include "LightingManager.h"


LightingManager* LightingManager::Instance = nullptr;

void LightingManager::Create()
{
	assert(Instance == nullptr);

	Instance = new LightingManager();
}

void LightingManager::Destroy()
{
	assert(Instance != nullptr);

	SAFE_DELETE(Instance);
}

LightingManager* LightingManager::Get()
{
	return Instance;
}

LightingManager::LightingManager()
{
	LightCBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		PS_Lights,
		&LightData,
		"Light Buffer",
		sizeof(LightDesc),
		false
	);
}

LightingManager::~LightingManager()
{	
	SAFE_DELETE(LightCBuffer);
}

void LightingManager::AddPointLight(const PointLight& light)
{
	UINT & Count = LightData.PointLightData.Count;
	if (Count < MAX_POINT_LIGHTS - 1)
		LightData.PointLightData.Lights[Count++] = light;
}

void LightingManager::AddSpotLight(const SpotLight& light)
{
	UINT & Count = LightData.SpotLightData.Count;
	if (Count < MAX_SPOT_LIGHTS - 1)
		LightData.SpotLightData.Lights[Count++] = light;
}

void LightingManager::Tick()
{
	if (LightData.PointLightData.Count > 0)
	{
		ImGui::Separator();
		ImGui::SeparatorText("PointLight");

		static int index = 0;
		ImGui::InputInt("PL_Index", &index);
		index %= static_cast<int>(LightData.PointLightData.Count);

		
		// ImGui::SliderFloat3("PL_Position", LightData.PointLightData.Lights[index].Position, -1000, 1000);
		ImGui::InputFloat3("PL_Position", LightData.PointLightData.Lights[index].Position);
		ImGui::Separator();
		ImGui::SliderFloat("PL_Intensity", &LightData.PointLightData.Lights[index].Intensity, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat("PL_Range", &LightData.PointLightData.Lights[index].Range, 1.0f, 10);

		ImGui::Separator();
		ImGui::ColorEdit3("PL_Ambient", LightData.PointLightData.Lights[index].Ambient);
		ImGui::ColorEdit3("PL_Diffuse", LightData.PointLightData.Lights[index].Diffuse);
		ImGui::ColorEdit3("PL_Specular", LightData.PointLightData.Lights[index].Specular);
		ImGui::ColorEdit3("PL_Emissive", LightData.PointLightData.Lights[index].Emissive);	
	}
	
	if (LightData.SpotLightData.Count > 0)
	{
		ImGui::Separator();
		ImGui::SeparatorText("_SpotLight");

		static int index = 0;
		ImGui::InputInt("SL_Index", &index);
		index %= static_cast<int>(LightData.SpotLightData.Count);

		ImGui::InputFloat3("SL_Position", LightData.SpotLightData.Lights[index].Position);
		ImGui::InputFloat3("SL_Direction", LightData.SpotLightData.Lights[index].Direction);

		
		ImGui::Separator();

		ImGui::SliderFloat("SL_Intensity", &LightData.SpotLightData.Lights[index].Intensity, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat("SL_Range", &LightData.SpotLightData.Lights[index].Range, 1.0f, 10);
		ImGui::SliderFloat("SL_Angle(Theta)", &LightData.SpotLightData.Lights[index].Theta, 1.0f, 180);
		ImGui::SliderFloat("SL_Angle(Phi)", &LightData.SpotLightData.Lights[index].Phi, LightData.SpotLightData.Lights[index].Theta, 180);
		
		ImGui::Separator();

		ImGui::ColorEdit3("SL_Ambient", LightData.SpotLightData.Lights[index].Ambient);
		ImGui::ColorEdit3("SL_Diffuse", LightData.SpotLightData.Lights[index].Diffuse);
		ImGui::ColorEdit3("SL_Specular", LightData.SpotLightData.Lights[index].Specular);
		ImGui::ColorEdit3("SL_Emissive", LightData.SpotLightData.Lights[index].Emissive);	
	}
	LightCBuffer->UpdateData(&LightData, sizeof(LightDesc));
}

void LightingManager::Render()
{
	LightCBuffer->BindToGPU();
}

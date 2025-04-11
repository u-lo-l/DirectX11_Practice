#include "framework.h"
#include "Shadow.h"

Shadow::Shadow(const Vector& InPosition, float InRadius, UINT InWidth, UINT InHeight)
	: Position(InPosition), Radius(InRadius)
{
	ShadowRenderTarget = new RenderTarget(InWidth, InHeight);
	ShadowDepthStencil = new DepthStencil(InWidth, InHeight, false);
	ShadowViewport = new ViewPort(static_cast<float>(InWidth), static_cast<float>(InHeight));
	ShadowData_PS = {
		{static_cast<float>(InWidth), static_cast<float>(InHeight)},
		0.01,
		0
	};
	
	ShadowViewProjectionCBuffer = new ConstantBuffer(
		ShaderType::VertexShader,
		Slot_ShadowData_VS,
		&ShadowData_VS,
		"Shadow Data",
		sizeof(ShadowData_VS_Desc),
		false
	);
	ShadowDataCBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		Slot_ShadowData_PS,
		&ShadowData_PS,
		"Shadow Data",
		sizeof(ShadowData_PS_Desc),
		false
	); 
}

Shadow::~Shadow()
{
	SAFE_DELETE(ShadowRenderTarget);
	SAFE_DELETE(ShadowDepthStencil);
	SAFE_DELETE(ShadowViewport);
}

void Shadow::Tick()
{
	ImGui::Separator();
	ImGui::SeparatorText("Shadow");
	ImGui::SliderFloat("Shadow Bias", &ShadowData_PS.ShadowBias, -0.01f, +0.01f);
	
	ImGui::InputInt("Shadow Quality", (int*)&ShadowData_PS.ShadowLevel);
	ShadowData_PS.ShadowLevel %= 5;

	ImGui::InputFloat("Shadow Radius", &Radius, 1);

	Vector direction = Context::Get()->GetLightDirection();
	Vector position = direction * Radius * -2.0f;

	ShadowData_VS.View = Matrix::CreateLookAt(position, Position, Vector::Up);

	Vector origin = Vector::TransformCoord(Position, ShadowData_VS.View);

	float l = origin.X - Radius;
	float b = origin.Y - Radius;
	float n = origin.Z - Radius;

	float r = origin.X + Radius;
	float t = origin.Y + Radius;
	float f = origin.Z + Radius;

	ShadowData_VS.Projection = Matrix::CreateOrthographic(r - l, t - b, n, f);
	ShadowViewProjectionCBuffer->UpdateData(&ShadowData_VS, sizeof(ShadowData_VS_Desc));
	ShadowDataCBuffer->UpdateData(&ShadowData_PS, sizeof(ShadowData_PS_Desc));
}

void Shadow::BindToGPU()
{
	ShadowViewProjectionCBuffer->BindToGPU();
	ShadowDataCBuffer->BindToGPU();
}

void Shadow::PreRender()
{
	ShadowRenderTarget->SetRenderTarget(ShadowDepthStencil);
	ShadowRenderTarget->ClearRenderTarget(Color::White);
	ShadowDepthStencil->ClearDepthStencil();
	ShadowViewport->RSSetViewport();
}


#include "framework.h"

GlobalViewProjectionCBuffer::GlobalViewProjectionCBuffer()
{
	ViewProjectionBuffer = new ConstantBuffer(
		ShaderType::VertexShader,
		VS_ViewProjection,
		nullptr,
		"ViewProjection",
		sizeof(ViewProjectionDesc),
		false
	);
	LightDirectionBuffer = new ConstantBuffer(
		ShaderType::PixelShader,
		PS_LightDirection,
		nullptr,
		"LightDirection",
		sizeof(LightDirectionDesc),
		false
	);
}

GlobalViewProjectionCBuffer::~GlobalViewProjectionCBuffer()
{
	SAFE_DELETE( ViewProjectionBuffer );
	SAFE_DELETE( LightDirectionBuffer );
}

void GlobalViewProjectionCBuffer::Tick()
{
	ViewProjectionData.View = Context::Get()->GetViewMatrix();
	ViewProjectionData.ViewInv = Matrix::Invert(ViewProjectionData.View);
	ViewProjectionData.Projection = Context::Get()->GetProjectionMatrix();
	ViewProjectionBuffer->UpdateData(&ViewProjectionData, sizeof(ViewProjectionDesc));

	LightDirectionData.LightDirection = Context::Get()->GetLightDirection();
	LightDirectionBuffer->UpdateData(&LightDirectionData, sizeof(LightDirectionDesc));
}

void GlobalViewProjectionCBuffer::BindToGPU() const
{
	ViewProjectionBuffer->BindToGPU();
	LightDirectionBuffer->BindToGPU();
}

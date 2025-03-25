#include "framework.h"

UINT GlobalViewProjectionCBuffer::Count = 0;

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
	GlobalViewProjectionCBuffer::Count++;
}

GlobalViewProjectionCBuffer::~GlobalViewProjectionCBuffer()
{
	GlobalViewProjectionCBuffer::Count--;
	SAFE_DELETE( ViewProjectionBuffer );
	SAFE_DELETE( LightDirectionBuffer );
}

void GlobalViewProjectionCBuffer::Tick()
{
	// ContextDescData.View = Context::Get()->GetViewMatrix();
	// ContextDescData.ViewInv = Matrix::Invert(ContextDescData.View);
	// ContextDescData.Projection = Context::Get()->GetProjectionMatrix();
	// ContextDescData.ViewProjection = ContextDescData.View * ContextDescData.Projection;
	// ContextDescData.LightDirection = Context::Get()->GetLightDirection();

	ViewProjectionData.View = Context::Get()->GetViewMatrix();
	ViewProjectionData.Projection = Context::Get()->GetProjectionMatrix();
	LightDirectionData.LightDirection = Context::Get()->GetLightDirection();

	ViewProjectionBuffer->UpdateData(&ViewProjectionData, sizeof(ViewProjectionDesc));
	LightDirectionBuffer->UpdateData(&LightDirectionData, sizeof(LightDirectionDesc));
}

void GlobalViewProjectionCBuffer::BindToGPU() const
{
	ViewProjectionBuffer->BindToGPU();
	LightDirectionBuffer->BindToGPU();
}

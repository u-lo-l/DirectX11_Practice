#include "framework.h"

UINT GlobalViewProjectionCBuffer::Count = 0;

GlobalViewProjectionCBuffer::GlobalViewProjectionCBuffer()
{
	const string DataInfo = "World Context Desc #" + to_string(GlobalViewProjectionCBuffer::Count);
	ViewProjectionBuffer = new ConstantBuffer(
												ShaderType::VertexShader,
												VS_ViewProjection,
												nullptr,
												DataInfo,
												sizeof(ViewProjectionDesc),
												false
											);
	
	GlobalViewProjectionCBuffer::Count++;
}

GlobalViewProjectionCBuffer::~GlobalViewProjectionCBuffer()
{
	GlobalViewProjectionCBuffer::Count--;
	SAFE_DELETE( ViewProjectionBuffer );
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
	ViewProjectionData.LightDirection = Context::Get()->GetLightDirection();

	ViewProjectionBuffer->UpdateData(&ViewProjectionData, sizeof(ViewProjectionDesc));
}

void GlobalViewProjectionCBuffer::BindToGPU() const
{
	ViewProjectionBuffer->BindToGPU();
}

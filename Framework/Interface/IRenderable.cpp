#include "framework.h"
#include "IRenderable.h"

IRenderable::IRenderable(Camera * InCamera)
	: TargetCamera(InCamera)
{
	if (InCamera == nullptr)
		TargetCamera = Context::Get()->GetCamera();
	UpdateCameraMatrix();
	string DataInfo = "WVP Matrix ";
	WVP_CBuffer = new ConstantBuffer(ShaderType::VertexShader, 1, &WVP_Matrix, DataInfo, sizeof(WVPDesc));
}

IRenderable::~IRenderable()
{
	SAFE_DELETE(WVP_CBuffer);
}

void IRenderable::SetCamera( Camera * InCamera )
{
	TargetCamera = InCamera;
}

void IRenderable::Tick()
{
	UpdateCameraMatrix();
	if (!!WVP_CBuffer)
	{
		WVP_CBuffer->UpdateData(&WVP_Matrix, sizeof(WVP_Matrix));
	}
}

void IRenderable::Render() const
{
	if (!!WVP_CBuffer)
	{
		WVP_CBuffer->BindToGPU();
	}
}

void IRenderable::UpdateCameraMatrix()
{
	if (!!TargetCamera)
	{
		WVP_Matrix.View = TargetCamera->GetViewMatrix();
		WVP_Matrix.Projection = TargetCamera->GetViewMatrix();
	}
}

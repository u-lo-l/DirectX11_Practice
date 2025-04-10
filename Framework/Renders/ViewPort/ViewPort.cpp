#include "framework.h"
#include "ViewPort.h"

ViewPort::ViewPort(float InWidth, float InHeight, float InX, float InY, float InMinDepth, float InMaxDepth)
{
	SetViewPort(InWidth, InHeight, InX, InY, InMinDepth, InMaxDepth);
}

void ViewPort::SetViewPort(float InWidth, float InHeight, float InX, float InY, float InMinDepth, float InMaxDepth)
{
	Vp.TopLeftX = InX;
	Vp.TopLeftY = InY;
	Vp.Width = InWidth;
	Vp.Height = InHeight;
	Vp.MinDepth = InMinDepth;
	Vp.MaxDepth = InMaxDepth;
	RSSetViewport();
}

void ViewPort::RSSetViewport() const
{
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, &Vp);
}

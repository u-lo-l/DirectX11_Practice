#pragma once

class ViewPort
{
public:
	ViewPort(float InWidth, float InHeight, float InX = 0, float InY = 0, float InMinDepth = 0, float InMaxDepth = 1);
	~ViewPort() = default;
	void SetViewPort(float InWidth, float InHeight, float InX = 0, float InY = 0, float InMinDepth = 0, float InMaxDepth = 1);
	void RSSetViewport() const;
private:
	D3D11_VIEWPORT Vp;
};

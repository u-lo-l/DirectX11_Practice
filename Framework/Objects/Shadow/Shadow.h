#pragma once

class Shadow
{
public:

	DepthStencil * ShadowDepthStencil;
	RenderTarget * ShadowRenderTarget;
	D3D11_VIEWPORT ShadowViewport;
		
};

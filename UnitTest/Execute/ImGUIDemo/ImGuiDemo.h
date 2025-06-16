#pragma once
#include "Systems/IExecutable.h"

class ImGuiDemo : public IExecutable
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void PostRender() override;
private:
	Texture * T = nullptr;
	Hlsl2DTextureShader * Shader = nullptr;

	RenderTarget * ImGuiRT = nullptr;
	DepthStencil * ImGuiDS = nullptr;
};

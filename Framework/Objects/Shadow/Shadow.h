#pragma once

class Shadow
{
	constexpr static int Slot_ShadowData_VS = 10;
	constexpr static int Slot_ShadowData_PS = 11;
public:
	Shadow(const Vector& InPosition, float InRadius, UINT InWidth = 1024, UINT InHeight = 1024);
	~Shadow();

	float GetWidth() const { return ShadowData_PS.ShadowMapSize.X; }
	float GetHeight() const { return ShadowData_PS.ShadowMapSize.Y; }
	RenderTarget * GetRenderTarget() const { return ShadowRenderTarget;}
	DepthStencil * GetDepthStencil() const { return ShadowDepthStencil;}
	
	void Tick();
	void BindToGPU();
	void PreRender();

private:
	struct ShadowData_VS_Desc
	{
		Matrix View;
		Matrix Projection;

	} ShadowData_VS;

	struct ShadowData_PS_Desc
	{
		Vector2D ShadowMapSize = {0,0};
		float ShadowBias = 0.05f;
		int ShadowLevel = 0;
	} ShadowData_PS;


	RenderTarget * ShadowRenderTarget;
	DepthStencil * ShadowDepthStencil;
	ViewPort * ShadowViewport;

	Vector Position;
	float Radius;

	ConstantBuffer * ShadowViewProjectionCBuffer = nullptr;
	ConstantBuffer * ShadowDataCBuffer = nullptr;
};

#pragma once

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;

	float Width;
	float Height;
	bool bVSync;

	Color Background;
};

class D3D
{
public:
	static D3D* Get();

public:
	static void Create();
	static void Destroy();

public:
	static const D3DDesc & GetDesc();
	static void SetDesc(const D3DDesc& InDesc);

private:
	D3D();
	~D3D();
	void CreateDevice();
	void CreateRTV(); // RTV : Render Target View
private:
	static D3D * Instance;

private:
	static D3DDesc D3dDesc;

private:
	ID3D11Device * Device;
	IDXGISwapChain * SwapChain;
	ID3D11DeviceContext* DeviceContext;

	ID3D11RenderTargetView * RenderTargetView;
};
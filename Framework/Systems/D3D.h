#pragma once

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;

	float Width;
	float Height;

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
	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);

private:
	void CreateDevice();
	void CreateRTV();
	void CreateViewport();

public:
	ID3D11Device* GetDevice() { return Device; }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext; }

	void ClearRenderTargetView(Color InColor);
	void Present();

private:
	D3D();
	~D3D();

private:
	static D3D* Instance;

private:
	static D3DDesc D3dDesc;

private:
	IDXGISwapChain* SwapChain;

	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;

	ID3D11RenderTargetView* RenderTargetView;

	D3D11_VIEWPORT* Viewport;
};


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

public :
	ID3D11Device* GetDevice() const { return this->Device; }
	ID3D11DeviceContext * GetDeviceContext() const{ return this->DeviceContext; }
	// IDXGISwapChain* GetSwapChain() const { return this->SwapChain; }

	/***
	 * InColor : Bac
	 ***/
	void ClearRTV(Color InColor);
	/***
	 * BackBuffer에 있는 것들을 보여주라는 명령
	 ***/
	void Present();
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
	IDXGISwapChain* SwapChain;
	ID3D11Device * Device;
	ID3D11DeviceContext* DeviceContext;
	ID3D11RenderTargetView * RenderTargetView;
};
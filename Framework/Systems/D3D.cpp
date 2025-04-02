#include "Framework.h"
#include "D3D.h"

D3D * D3D::Instance = nullptr;

D3DDesc D3D::D3dDesc = D3DDesc();

D3D * D3D::Get()
{
	assert(Instance != nullptr);
	return Instance;
}

void D3D::Create()
{
	assert(Instance == nullptr);
	Instance = new D3D();
}

void D3D::Destroy()
{
	SAFE_DELETE(Instance);
}

const D3DDesc & D3D::GetDesc()
{
	return D3dDesc;
}

void D3D::SetDesc( const D3DDesc & InDesc )
{
	D3dDesc = InDesc;
}

void D3D::ClearRenderTargetView( const Color & InColor ) const
{
	DeviceContext->ClearRenderTargetView(RenderTargetView, InColor);
}

void D3D::ClearDepthStencilView() const
{
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void D3D::Present() const
{
	HRESULT hr = SwapChain->Present(0, 0);
	CHECK(hr >= 0);
}

void D3D::ResizeScreen( float InWidth, float InHeight )
{
	if (InWidth < 1 || InHeight < 1) // 최소화 모드
	{
		return;
	}
	D3dDesc.Width = InWidth;
	D3dDesc.Height = InHeight;

	SAFE_RELEASE(RenderTargetView);

	const HRESULT Hr = SwapChain->ResizeBuffers(0, static_cast<UINT>(InWidth), static_cast<UINT>(InHeight), DXGI_FORMAT_UNKNOWN, 0);
	CHECK(Hr >= 0);

	CreateRTV();
	CreateDSV();
	SetRenderTarget();
}

D3D::D3D()
{
	CreateDeviceAndContext();
	CreateRTV();
	CreateDSV();
	SetRenderTarget();
}

D3D::~D3D()
{
	SAFE_RELEASE(DSVTexture);
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);
	SAFE_RELEASE(SwapChain);
}

void D3D::CreateDeviceAndContext()
{
	//Create Device And SwapChain
	{
		// 디스플레이 모드 및 디스플레이 모드에서 스테레오를 지원하는지 여부를 설명하는 객체
		DXGI_MODE_DESC Desc;
		ZeroMemory(&Desc, sizeof(DXGI_MODE_DESC));

		Desc.Width = static_cast<UINT>(D3dDesc.Width);
		Desc.Height = static_cast<UINT>(D3dDesc.Height);
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		Desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		Desc.RefreshRate.Numerator = 0;
		Desc.RefreshRate.Denominator = 1;

		// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-create-ref
		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		SwapChainDesc.BufferDesc = Desc;
		// SampleDesc : 다중 샘플링 매개변수
		SwapChainDesc.SampleDesc.Count = 1;		// pixel당 MultiSample개수
		SwapChainDesc.SampleDesc.Quality = 0;	//
		// BufferUsage : https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/dxgi-usage
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Output RenderTarget으로 설정
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.OutputWindow = D3dDesc.Handle;
		SwapChainDesc.Windowed = true; // true(창모드)

		D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
		// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-initialize
		const HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			//https://learn.microsoft.com/ko-kr/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&SwapChainDesc,
			&SwapChain,
			&Device,
			&FeatureLevels,
			&DeviceContext
		);
		ASSERT(hr >= 0, "Device Failed")
	}
}

void D3D::CreateRTV()
{
	ID3D11Texture2D * BackBuffer;
	HRESULT Hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&BackBuffer));
	ASSERT(Hr >= 0, "Back Buffer Failed")

	Hr = Device->CreateRenderTargetView(BackBuffer, nullptr, &RenderTargetView);
	ASSERT(Hr >= 0, "RTV Failed")

	SAFE_RELEASE(BackBuffer);
}

void D3D::CreateDSV()
{
	D3D11_TEXTURE2D_DESC DepthStencilBufferDecs;
	ZeroMemory(&DepthStencilBufferDecs, sizeof(DepthStencilBufferDecs));
	DepthStencilBufferDecs.Width = static_cast<UINT>(D3dDesc.Width);
	DepthStencilBufferDecs.Height = static_cast<UINT>(D3dDesc.Height);
	DepthStencilBufferDecs.MipLevels = 1;
	DepthStencilBufferDecs.ArraySize = 1;
	// DepthStencilBufferDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilBufferDecs.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilBufferDecs.SampleDesc.Count = 1;
	DepthStencilBufferDecs.SampleDesc.Quality = 0;
	DepthStencilBufferDecs.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilBufferDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilBufferDecs.CPUAccessFlags = 0;
	DepthStencilBufferDecs.MiscFlags = 0;
	CHECK(Device->CreateTexture2D(&DepthStencilBufferDecs, nullptr, &DSVTexture)>= 0);

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));
	DepthStencilViewDesc.Format = DepthStencilBufferDecs.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	CHECK(Device->CreateDepthStencilView(DSVTexture, &DepthStencilViewDesc, &DepthStencilView) >= 0);
}

void D3D::SetRenderTarget() const
{
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
}

#include "Framework.h"
#include "D3D.h"

D3D * D3D::Instance = nullptr;

D3DDesc D3D::D3dDesc = D3DDesc();


D3D* D3D::Get()
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

void D3D::SetDesc(const D3DDesc& InDesc)
{
	D3dDesc = InDesc;
}

void D3D::ClearRenderTargetView(Color InColor) const
{
	DeviceContext->ClearRenderTargetView(RenderTargetView, InColor.ToDx());
}

void D3D::Present() const
{
	SwapChain->Present(0, 0);
}

void D3D::ResizeScreen(float InWidth, float InHeight)
{
	if (InWidth < 1 || InHeight < 1) // 최소화 모드
		return;
	D3D::D3dDesc.Width = InWidth;
	D3D::D3dDesc.Height = InHeight;

	SAFE_RELEASE(RenderTargetView);

	const HRESULT Hr = SwapChain->ResizeBuffers(0, static_cast<UINT>(InWidth), static_cast<UINT>(InHeight), DXGI_FORMAT_UNKNOWN, 0);
	CHECK(Hr >= 0);

	CreateRTV();
}

D3D::D3D()
{
	CreateDeviceAndImmediateContext();
	CreateRTV();
}

D3D::~D3D()
{
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);
	SAFE_RELEASE(SwapChain);
}

void D3D::CreateDeviceAndImmediateContext()
{
	//Create Device And SwapChain
	{
		// 디스플레이 모드 및 디스플레이 모드에서 스테레오를 지원하는지 여부를 설명하는 객체
		DXGI_MODE_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_MODE_DESC));

		desc.Width = (UINT)D3D::D3dDesc.Width;
		desc.Height = (UINT)D3D::D3dDesc.Height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.RefreshRate.Numerator = 0;
		desc.RefreshRate.Denominator = 1;

		// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-create-ref
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc = desc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		swapChainDesc.OutputWindow = D3D::D3dDesc.Handle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = true;


		/*
		pAdapter
		   Video Adapter에 대한 포인터이다.
		   nullptr전달 시 기본 어댑터가 사용된다.
		DriverType
		   드라이버 유형을 나타낸다.
		   예: D3D_DRIVER_TYPE_HARDWARE(GPU 사용),
		      D3D_DRIVER_TYPE_WARP (소프트웨어 래스터라이저 사용),
		      D3D_DRIVER_TYPE_REFERENCE (레퍼런스 드라이버 사용).
		Software
		    소프트웨어 렌더링 드라이버를 지정할 때 사용.
		    D3D_DRIVER_TYPE_SOFTWARE가 설정된 경우에만 유효하며, 그렇지 않으면 nullptr로 설정.
		Flags
		   디바이스 생성에 사용되는 플래그를 나타낸다.
		   예: D3D11_CREATE_DEVICE_DEBUG (디버그 레이어 활성화),
		      D3D11_CREATE_DEVICE_SINGLETHREADED (싱글 스레드 모드).
		pFeatureLevels
		   지원할 Feature Level의 배열 포인터.
		   nullptr인 경우 DirectX는 GPU에서 지원하는 최상위 Feature Level을 자동으로 선택한다.
		FeatureLevels
		   pFeatureLevels 배열의 크기(지원할 Feature Level의 개수).
		   pFeatureLevels가 nullptr이면 이 값은 무시된다.
		SDKVersion
		   SDK 버전을 나타낸다.
		   항상 D3D11_SDK_VERSION을 사용해야 한다.
		pSwapChainDesc
		[in]
		   SwapChain의 설정정보를 담은 구조체포인터를 전달한다.
		   nullptr이면 SwapChain이 생성되지 않는다.
		ppSwapChain
		[out]
		   생성된 IDXGISwapChain인터페이스의 포인터를 반환한다.
		ppDevice
		[out]
		   생성된 ID3D11Device에 대한 포인터를 반환한다.
		pFeatureLevel
		[out]
		   생성된 디바이스에서 지원하는 Feature Level을 반환한다.
		   null이면 GPU가 지원하는 가장 높은 Feature Level이 설정된다.
		   DirectX의 기능 수준을 확인하거나 특정 기능 사용 가능 여부를 결정한다.
		ppImmediateContext
		[out]
		   생성된 ID3D11DeviceContext 인터페이스에 대한 포인터를 반환한다.
		   GPU 명령을 실행하는데 사용되는 실행 컨택스트
		   -> 즉시 실행 컨텍스트
		      주로 싱글 스레드 환경에서 사용된다.
		   -> 지연 실행 컨텍스트 (Deferred)
		      명령을 다른 스레드에서 준비하고, 즉시 실행 컨텍스트에서 실행된다.
		      컨텍스트는 일반적으로 작업을 실행하는 데 필요한 
		      환경정보를 포함하는 객체이다.
		   DX11에서는 주로 ID3D11DeviceContext객체를 의미한다.
		 */
		D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
		// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-initialize
		const HRESULT hr = D3D11CreateDeviceAndSwapChain
		(
			nullptr,
			//https://learn.microsoft.com/ko-kr/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
			D3D_DRIVER_TYPE_HARDWARE, 
			nullptr, 
			0, 
			nullptr, 
			0, 
			D3D11_SDK_VERSION, 
			&swapChainDesc, 
			&SwapChain, 
			&Device, 
			&FeatureLevels,
			&DeviceContext
		);
		ASSERT(hr >= 0, "Device Failed");
	}
}

void D3D::CreateRTV()
{
	ID3D11Texture2D* backBuffer;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	ASSERT(hr >= 0, "Back Buffer Failed");

	hr = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
	ASSERT(hr >= 0, "RTV Failed");

	SAFE_RELEASE(backBuffer);

	// pDepthStencilView에 nullptr을 전달한다는 것은 BackBuffer를 RenderTarget으로 쓰겠다는 의미.
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);
}

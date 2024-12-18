#include "Framework.h"
#include "D3D.h"

D3D* D3D::Instance = nullptr;

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

const D3DDesc& D3D::GetDesc()
{
	return D3dDesc;
}

void D3D::SetDesc(const D3DDesc& InDesc)
{
	D3dDesc = InDesc;
}

D3D::D3D() = default;

D3D::~D3D()= default;

void D3D::CreateDevice()
{
	/*
	 * Create Device And SwapChain
	 * Device란?
	 *  GPU와 통신하는 오브젝트이다.
	 *  텍스쳐, 버퍼, 쉐이더 등의 리소스를 관리한다.
	 *  렌더링 파이프라인을 관리하다.
	 *  
	 * SwapChain이란?
	 *	프론트 버퍼와 백 버퍼를 사용하여 화면 출력을 관리하는 DX 구성요소이다.
	 *	더블 버펄이 혹은 트리플 버퍼링을 활용하여 깜빡임을 방지한다.
	 *	VSync와 같은 옵션을 통해 디스플레이와 GPU를 동기화 하여 화면 찢김을 방지할 수 있다.
	 *	BackBuffer에 렌더링한다. 렌더링이 완료되면 프론트 버퍼와 백 버퍼를 교체한다.
	 *	
	 * 실행컨텍스트란?
	 *	GPU명령을 즉시 실행하게 해주는 DX 오브젝트이다.
	 *	랜더링 명령, 쉐이더 실행, 리소스 업데이트 등의 GPU작업을 가능한 빠르게(즉시) 처리한다.
	 */
	{
		DXGI_MODE_DESC Desc;
		ZeroMemory(&Desc, sizeof(DXGI_MODE_DESC));

		Desc.Width = static_cast<UINT>(D3D::D3dDesc.Width);
		Desc.Height = static_cast<UINT>(D3D::D3dDesc.Height);
		Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		Desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		Desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		Desc.RefreshRate.Numerator = 0; // 분모
		Desc.RefreshRate.Denominator = 1; // 분자

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		SwapChainDesc.BufferDesc = Desc;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;

		SwapChainDesc.OutputWindow = D3D::D3dDesc.Handle;

		/// <param name='pAdapter'>
		///		Video Adpter에 대한 포인터이다.
		///		nullptr전달 시 기본 어댑터가 사용된다.
		/// </param>
		/// <param name='DriverType'>
		///		드라이버 유형을 나타낸다.
		///		예:	D3D_DRIVER_TYPE_HARDWARE(GPU 사용),
		///			D3D_DRIVER_TYPE_WARP (소프트웨어 래스터라이저 사용),
		///			D3D_DRIVER_TYPE_REFERENCE (레퍼런스 드라이버 사용).
		/// </param>
		/// <param name='Software'>
		/// 	소프트웨어 렌더링 드라이버를 지정할 때 사용.
		///		D3D_DRIVER_TYPE_SOFTWARE가 설정된 경우에만 유효하며, 그렇지 않으면 nullptr로 설정.
		/// </param>
		/// <param name='Flags'>
		///		디바이스 생성에 사용되는 플래그를 나타낸다.
		///		예:	D3D11_CREATE_DEVICE_DEBUG (디버그 레이어 활성화),
		///			D3D11_CREATE_DEVICE_SINGLETHREADED (싱글 스레드 모드).
		/// </param>
		/// <param name='pFeatureLevels'>
		///		지원할 Feature Level의 배열 포인터.
		///		nullptr인 경우 DirectX는 GPU에서 지원하는 최상위 Feature Level을 자동으로 선택한다.
		/// </param>
		/// <param name='FeatureLevels'>
		///		pFeatureLevels 배열의 크기(지원할 Feature Level의 개수).
		///		pFeatureLevels가 nullptr이면 이 값은 무시된다.
		/// </param>
		/// <param name='SDKVersion'>
		///		SDK 버전을 나타낸다.
		///		항상 D3D11_SDK_VERSION을 사용해야 한다.
		/// </param>
		/// <param name='pSwapChainDesc'>
		/// [in]
		///		SwapChain의 설정정보를 담은 구조체포인터를 전달한다.
		///		nullptr이면 SwapChain이 생성되지 않는다.
		/// </param>
		/// <param name='ppSwapChain'>
		/// [out]
		///		생성된 IDXGISwapChain인터페이스의 포인터를 반환한다.
		/// </param>
		/// <param name='ppDevice'>
		/// [out]
		///		생성된 ID3D11Device에 대한 포인터를 반환한다.
		/// </param>
		/// <param name='pFeatureLevel'>
		/// [out]
		///		생성된 디바이스에서 지원하는 Feature Level을 반환한다.
		///		GPU가 지원하는 가장 높은 Feature Level이 설정된다.
		///		DirectX의 기능 수준을 확인하거나 특정 기능 사용 가능 여부를 결정한다.
		/// </param>
		/// <param name='ppImmediateContext'>
		/// [out]
		///		생성된 ID3D11DeviceContext 인터페이스에 대한 포인터를 반환한다.
		///		GPU 명령을 실행하는데 사용되는 실행 컨택스트
		///		-> 즉시 실행 컨텍스트
		///			주로 싱글 스레드 환경에서 사용된다.
		///		-> 지연 실행 컨텍스트 (Deferred)
		///			명령을 다른 스레드에서 준비하고, 즉시 실행 컨텍스트에서 실행된다.
		///		컨텍스트는 일반적으로 작업을 실행하는 데 필요한 환경정보를 포함하는 객체이다.
		///		DX11에서는 주로 ID3D11DeviceContext객체를 의미한다.
		/// </param>
		const HRESULT Hresult = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			1,
			D3D11_SDK_VERSION,
			&SwapChainDesc,
			&SwapChain,
			&Device,
			nullptr,
			&DeviceContext
		);
		assert(Hresult >= 0 && "Create Device FAIL");
	}
}
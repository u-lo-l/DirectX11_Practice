#pragma once
#include "../Core/Color.h"


/// <summary>
/// DirectX11 프로젝트를 위한 기본(커스텀) 구조체이다.
/// 각 필드는 애플리케이션의 이름, 윈도우 핸들, 화면 크기 및 배경색과 같은 주요 속성을 포함한다.
/// </summary>
/// <remarks>
/// 이 구조체는 DirectX11 애플리케이션의 초기화와 설정 정보를 저장하는 데 사용된다.
/// - Desc : Description으로 구조체가 어떤 데이터를 설정하는 역할을 하는 것을 의미한다.
/// </remarks>
/// <description> - AppName : 애플리케이션의 이름을 저장한다.</description>
/// <description> - Instance : Win32 애플리케이션의 인스턴스 핸들을 저장한다.</description>
/// <description> - Handle : 윈도우 핸들(HWND)을 저장한다. 초기값은 nullptr로 설정된다.</description>
/// <description> - Width : 애플리케이션 창의 너비를 나타낸다. 기본값은 픽셀 단위로 설정된다.</description>
/// <description> - Height : 애플리케이션 창의 높이를 나타낸다. 기본값은 픽셀 단위로 설정된다.</description>
/// <description> - Background : 애플리케이션의 기본 배경색을 나타낸다. RGBA 형식으로 정의된다.</description>
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
	static D3D * Get();

public:
	static void Create();
	static void Destroy();

public:
	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);
private:
	static D3D* Instance;
	static D3DDesc D3dDesc;

private:
	/// <summary> DirectX의 Device와 SwapChain을 생성. </summary>
	/// <remarks>
	/// - Device: GPU와 통신하는 객체로, Texture, Buffer, Shader 등의 리소스를 관리하며 렌더링 파이프라인에 관여한다.<br/>
	/// - SwapChain: FrontBuffer와 BackBuffer를 사용하여 출력을 관리하는 DirectX 구성 요소이다.<br/>
	/// - 실행 컨텍스트: GPU 명령을 실행시키는 DirectX 객체로, 작업을 실행하는 데 필요한 환경 정보를 포함하는 객체를 의미한다.<br/>
	///   - 즉시 실행 컨텍스트(Immediate Context): 싱글 스레드 환경에서 사용된다.<br/>
	///   - 지연 실행 컨텍스트(Deferred Context): 다른 스레드에서 명령을 준비하여 실행한다.
	/// </remarks>
	void CreateDeviceAndContext();

	/// <summary> DirectX11에서 렌더 타겟 뷰(Render Target View, RTV)를 생성. </summary>
	/// <remarks>
	/// 이 함수는 스왑 체인에서 백 버퍼를 가져와 렌더 타겟 뷰를 생성하고,
	/// 이를 Output-Merger Stage에서 렌더 타겟으로 설정한다.
	///  - Render Target : GPU가 수행한 렌더링 결과가 저장되는 공간. 화면에 직접 출력되거나 PostProcessing에 사용된다.
	///  BackBuffer가 가장 기본적인 Render Target이다.
	///  - Render Target View : 렌더 타깃을 GPU에 바인딩하기 위한 Direct3D 객체.
	///  - OM stage : 셰이더 처리 결과를 받아 최종적으로 출력 데이터(픽셀 색상, 깊이, 스텐실 값)를 결합.
	///  결합된 데이터를 렌더 타깃(Render Target)과 깊이-스텐실 뷰(Depth-Stencil View)에 출력
	/// </remarks>
	void CreateRTV();
	void CreateDSV();
	void BindRenderTargets() const;
public:
	ID3D11Device* GetDevice() const { return Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }

	void ClearRenderTargetView( const Color & InColor) const;
	void ClearDepthStencilView() const;
	void Present() const;
	void ResizeScreen(float InWidth, float InHeight);
private:
	D3D();
	~D3D();

private:
	// GPU와 통신하는 객체
	ID3D11Device * Device;
	IDXGISwapChain * SwapChain;
	// GPU 명령 즉시 실행 컨텍스트
	ID3D11DeviceContext * DeviceContext;
	ID3D11RenderTargetView * RenderTargetView;
	ID3D11DepthStencilView * DepthStencilView;
};


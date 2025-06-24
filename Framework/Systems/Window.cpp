#include "Framework.h"
#include <thread>
#include "Window.h"
#include "IExecutable.h"

std::thread * Window::GameThread = nullptr;
std::thread * Window::PhysicsThread = nullptr;
std::thread * Window::RenderThread = nullptr;

IExecutable * Window::Main = nullptr;

WPARAM Window::Run(IExecutable * InMain)
{
	srand(static_cast<UINT>(time(nullptr)));

	Create();
	D3D::Create();
	Gui::Create();
	Keyboard::Create();
	sdt::Mouse::Create();
	sdt::SystemTimer::Create();
	Context::Create();
	LightingManager::Create();
	ShaderManager::Create();
	RenderManager::Create();

	Window::GameThread = new std::thread(RunGameLogic);
	Window::PhysicsThread = new std::thread(RunPhysics);
	Window::RenderThread = new std::thread(RunRenderer);

	Main = InMain;
	Main->Initialize();

	while (true)
	{
		if (HandleOSEvent() == EXIT_FAILURE)
			break;
		MainRender();
	}

	Main->Destroy();
	LightingManager::Destroy();
	Context::Destroy();
	sdt::SystemTimer::Destroy();
	sdt::Mouse::Destroy();
	Keyboard::Destroy();
	Gui::Destroy();
	D3D::Destroy();
	Destroy();

	return 0;
}

void Window::Create()
{
	D3DDesc desc = D3D::GetDesc();

	//Register Window Class
	{
		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		// 윈도우가 메시지를 받을 때 호출되는 콜백함수
		// DispatchMessage가 호출될 떄 실행됨
		wndClass.lpfnWndProc = Window::WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = desc.Instance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 2); // ???
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = desc.AppName.c_str();

		const ATOM check = RegisterClassEx(&wndClass);
		CHECK(check != 0);
	}

	//Create Window Handle
	desc.Handle = CreateWindowEx
	(
		0,
		desc.AppName.c_str(),
		desc.AppName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, //Default - X
		CW_USEDEFAULT, //Default - Y
		static_cast<int>(desc.WindowWidth),
		static_cast<int>(desc.WindowHeight),
		nullptr,
		nullptr,
		desc.Instance,
		nullptr
	);
	CHECK(desc.Handle != nullptr);

	//렌더링 사이즈에 맞게 Window 창 크기 설정
	RECT WinRect = { 0,0, static_cast<long>(desc.WindowWidth), static_cast<long>(desc.WindowHeight)};
	const UINT WinCenterX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<UINT>(desc.WindowWidth)) / 2;
	const UINT WinCenterY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<UINT>(desc.WindowHeight)) / 2;
	AdjustWindowRect(&WinRect, WS_OVERLAPPEDWINDOW, FALSE);
	const UINT WinWidth = WinRect.right - WinRect.left;
	const UINT WinHeight = WinRect.bottom - WinRect.top;
	MoveWindow(desc.Handle, WinCenterX, WinCenterY, WinWidth, WinHeight, TRUE);

	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);
	ShowCursor(true);

	D3D::SetDesc(desc);
}

void Window::Destroy()
{
	const D3DDesc desc = D3D::GetDesc();

	GameThread->join();
	RenderThread->join();
	PhysicsThread->join();
	SAFE_DELETE(GameThread);
	SAFE_DELETE(PhysicsThread);
	SAFE_DELETE(RenderThread);
	
	DestroyWindow(desc.Handle);
	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

LRESULT CALLBACK Window::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	sdt::Mouse::Get()->WndProc(InMessage, InwParam, InlParam);

	if (Gui::Get()->WndProc(InHandle, InMessage, InwParam, InlParam))
		return TRUE;

	if (InMessage == WM_SIZE)
	{
		if (Main != nullptr)
		{
			const float Width  = LOWORD(InlParam);
			const float Height = HIWORD(InlParam);

			if (D3D::Get() != nullptr)
			{
				D3D::Get()->ResizeScreen(Width, Height);
			}
			if (Context::Get() != nullptr)
			{
				Context::Get()->ResizeScreen();
			}
		}
	}

	if (InMessage == WM_KEYDOWN)
	{
		if (InwParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	if (InMessage == WM_CLOSE || InMessage == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(InHandle, InMessage, InwParam, InlParam);
}

void Window::MainRender()
{
	Gui::Get()->Tick();
	sdt::SystemTimer::Get()->Tick();	// DeltaTime 계산
	sdt::Mouse::Get()->Tick();			// Mouse변화량 계산
	Context::Get()->Tick();				//
	LightingManager::Get()->Tick();
	Main->Tick();						// Main에 Push된 IExecutable들 실행
	RenderManager::Get()->Tick();
	
	// {
	// 	Main->PreRender();
	// }
	{
		D3D::Get()->SetRenderTarget();
		D3D::Get()->ClearRenderTargetView(D3D::GetDesc().Background);
		D3D::Get()->ClearDepthStencilView();
		LightingManager::Get()->Render();
		Context::Get()->Render();
		RenderManager::Get()->Render();
	}
	// {
	// 	Main->PostRender();
	// }
	Gui::Get()->Render();

	D3D::Get()->Present();
}

bool Window::HandleOSEvent()
{
	MSG Message;
	ZeroMemory(&Message, sizeof(MSG));
	if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message); //WM_CHAR
		DispatchMessage(&Message);
		if (Message.message == WM_QUIT)
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void Window::RunGameLogic()
{
	printf("Run Game Logic\n");
}

void Window::RunPhysics()
{
	printf("Run Physics Logic\n");
}

void Window::RunRenderer()
{
	printf("Run Render Logic\n");
}

﻿#include "Framework.h"
#include "Window.h"
#include "IExecutable.h"

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

	Main = InMain;
	Main->Initialize();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true) //Game Loop
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
	
			TranslateMessage(&msg); //WM_CHAR
			DispatchMessage(&msg);
		}
		else
		{
			MainRender();
		}
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
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = desc.Instance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 2);
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
		static_cast<int>(desc.Width),
		static_cast<int>(desc.Height),
		nullptr,
		nullptr,
		desc.Instance,
		nullptr
	);
	CHECK(desc.Handle != nullptr);

	//렌더링 사이즈에 맞게 Window 창 크기 설정
	RECT WinRect = { 0,0, static_cast<long>(desc.Width), static_cast<long>(desc.Height)};
	const UINT WinCenterX = (GetSystemMetrics(SM_CXSCREEN) - static_cast<UINT>(desc.Width)) / 2;
	const UINT WinCenterY = (GetSystemMetrics(SM_CYSCREEN) - static_cast<UINT>(desc.Height)) / 2;
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
	Gui::Tick();
	sdt::SystemTimer::Get()->Tick();	// DeltaTime 계산
	sdt::Mouse::Get()->Tick();			// Mouse변화량 계산
	Context::Get()->Tick();				//
	LightingManager::Get()->Tick();
	Main->Tick();						// Main에 Push된 IExecutable들 실행

	{
		Main->PreRender();	
	}
	
	{
		D3D::Get()->SetRenderTarget();
		D3D::Get()->ClearRenderTargetView(D3D::GetDesc().Background);
		D3D::Get()->ClearDepthStencilView();
		LightingManager::Get()->Render();
		Context::Get()->Render();
		Main->Render();
	}
	
	{
		Main->PostRender();	
		Gui::Get()->Render();
	}

	
	D3D::Get()->Present();
}

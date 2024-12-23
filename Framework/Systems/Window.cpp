#include "Framework.h"
#include "Window.h"
#include "IExecutable.h"

IExecutable* Window::Main = nullptr;

WPARAM Window::Run(IExecutable * InMain)
{
	Create();

	D3D::Create();

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

	D3D::Destroy();

	Destroy();

	return msg.wParam;
}

void Window::Create()
{
	D3DDesc desc = D3D::GetDesc();

	//Register Window Class
	{
		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = desc.Instance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 2));
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = desc.AppName.c_str();

		const ATOM check = RegisterClassEx(&wndClass);
		CHECK(static_cast<int>(check != 0));
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
	CHECK(static_cast<int>(desc.Handle != nullptr));

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

LRESULT Window::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
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
	//D3D::Get()->ClearRenderTargetView(Color(0, 0, 0, 0));
	D3D::Get()->ClearRenderTargetView(D3D::GetDesc().Background);

	Main->Tick();
	Main->Render();

	D3D::Get()->Present();
}

#include <Windows.h>
#include <cassert>
#include <string>

#include <d3dx10math.h>

using namespace std;

typedef D3DXCOLOR Color;

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

static LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	switch (InMessage)
	{
		case WM_KEYDOWN:
			if (InwParam == VK_ESCAPE)
			{
				//DestroyWindow(InHandle);
				PostQuitMessage(0);
				return 0;
			}

			if (InwParam == 0x41)
			{
				MessageBox(nullptr, L"�׽�Ʈ", L"Test", MB_OK);
				return 0;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(InHandle, InMessage, InwParam, InlParam);
	}

	
}

static void Create(D3DDesc& Desc)
{
	// Register Window Class
	{
		WNDCLASSEX WndClass;
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = WndProc;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = Desc.Instance;
		WndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		WndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		WndClass.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 2));
		WndClass.lpszMenuName = nullptr;
		WndClass.lpszClassName = Desc.AppName.c_str();

		const ATOM Check = RegisterClassEx(&WndClass);
		assert(Check != 0);
	}

	//Create Window Handle
	Desc.Handle = CreateWindowEx
	(
		0,
		Desc.AppName.c_str(),
		Desc.AppName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, //Default - X
		CW_USEDEFAULT, //Default - Y
		static_cast<int>(Desc.Width),
		static_cast<int>(Desc.Height),
		nullptr,
		nullptr,
		Desc.Instance,
		nullptr
	);
	assert(Desc.Handle != nullptr);

	ShowWindow(Desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(Desc.Handle);
	SetFocus(Desc.Handle);

	ShowCursor(true);
}

WPARAM Run(const D3DDesc& desc)
{
	//TODO: Create

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
			//TODO: Game Logic(Thread) - Tick
			//TODO: Game Render(Thread)
		}
	}


	//TODO: Destroy

	return msg.wParam;
}

int WINAPI WinMain(HINSTANCE InInstance, HINSTANCE InPrevInstance, LPSTR InParam, int command)
{
	D3DDesc Desc;
	Desc.AppName = L"D3D Game";
	Desc.Instance = InInstance;
	Desc.Handle = nullptr;

	Desc.Width = 1280;
	Desc.Height = 720;
	Desc.bVSync = false;

	Desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);

	Create(Desc);
	WPARAM wParam = Run(Desc);

	return wParam;
}
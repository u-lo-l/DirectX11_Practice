#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

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

	D3D::SetDesc(Desc);

	Main * main = new Main();
	WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main);

	return 0;
}
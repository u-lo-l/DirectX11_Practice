#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

#include "Line.h"

void Main::Initialize()
{
	Push(new Line());
}

void Main::Destroy()
{
	for (IExecutable* execute : executes)
	{
		execute->Destroy();

		SAFE_DELETE(execute);
	}
}

void Main::Tick()
{
	for (IExecutable* execute : executes)
		execute->Tick();
}

void Main::Render()
{
	for (IExecutable* execute : executes)
		execute->Render();
}

void Main::Push(IExecutable* execute)
{
	executes.push_back(execute);

	execute->Initialize();
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

	D3D::SetDesc(Desc);

	Main * main = new Main();
	WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main);

	return 0;
}
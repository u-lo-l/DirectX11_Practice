// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

#include "Line.h"
#include "Line2.h"

void Main::Initialize()
{
	//Push(new Line());
	Push(new Line2());
}

void Main::Destroy()
{
	for (IExecutable* executable : Executables)
	{
		executable->Destroy();

		SAFE_DELETE(executable)
	}
}

void Main::Tick()
{
	for (IExecutable* executable : Executables)
		executable->Tick();
}

void Main::Render()
{
	for (IExecutable* executable : Executables)
		executable->Render();
}

void Main::Push(IExecutable* Executable)
{
	Executables.push_back(Executable);

	Executable->Initialize();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = hInstance;
	desc.Handle = nullptr;

	desc.Width = 1280;
	desc.Height = 720;

	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);

	D3D::SetDesc(desc);


	Main * main = new Main();
	const WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main)

	return static_cast<int>(wParam);
}
// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"

#include "Execute/RawBufferDemo/RawBufferDemo.h"
// #include "Execute/004_Line/Line.h"
// #include "Execute/004_Line/Line2.h"
// #include "Execute/004_Line/Line_Circle.h"
#include "Execute/005_Triangle/Triangle_List.h"
#include "Execute/009_World/WorldDemo.h"
#include "Execute/009_World/WorldDemo2.h"
#include "Systems/Window.h"

void Main::Initialize()
{
	// PUSH_MAIN(Sdt::RawBufferDemo);
	// PUSH_MAIN(Line);
	// PUSH_MAIN(Line2);
	// PUSH_MAIN(Line_Circle);
	// PUSH_MAIN(Triangle_List);
	// PUSH_MAIN(Sdt::WorldDemo);
	PUSH_MAIN(Sdt::WorldDemo2);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = hInstance;
	desc.Handle = nullptr;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);

	D3D::SetDesc(desc);

	// ReSharper disable once CppUseAuto
	Main * main = new Main();
	const WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main)
	return static_cast<int>(wParam);
}
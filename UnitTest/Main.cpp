// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

#include "Execute/011_Grid/Grid.h"
#include "Execute/019_TextureColorBlending/TextureColorBlending.h"
#include "Execute/024_Terrain/TerrainDemo.h"
#include "Execute/ComputeShaderDemo/RawBufferDemo.h"
#include "Execute/InstancingDemo/InstancingDemo.h"

void Main::Initialize()
{
	// PUSH_MAIN(Sdt::RawBufferDemo);
	// PUSH_MAIN(Line);
	// PUSH_MAIN(Sdt::Line2);
	// PUSH_MAIN(Line_Circle);
	// PUSH_MAIN(Triangle_List);
	// PUSH_MAIN(Sdt::WorldDemo);
	// PUSH_MAIN(Sdt::WorldDemo2);
	// PUSH_MAIN(Sdt::Grid);
	// PUSH_MAIN(Sdt::TextureColorBlending);
	// PUSH_MAIN(Sdt::TerrainDemo);
	// PUSH_MAIN(sdt::InstancingDemo);
	PUSH_MAIN(sdt::RawBufferDemo);
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
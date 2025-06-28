// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"

#include <thread>

#include "Execute/CharacterMoveDemo/CharacterMoveDemo.h"
#include "Systems/Window.h"

void Main::Initialize()
{
	// PUSH_MAIN(sdt::PNTriangleDemo);
	// PUSH_MAIN(sdt::DisplacementMappingDemo);
	// PUSH_MAIN(sdt::BasicTessellationDemo);
	// PUSH_MAIN(sdt::TerrainTessellationDemo);
	// PUSH_MAIN(sdt::ShadowDemo);
	// PUSH_MAIN(sdt::TextureTransposeDemo);
	// PUSH_MAIN(sdt::PhilipsSpectrumDemo);
	// PUSH_MAIN(sdt::OceanDemo);
	// PUSH_MAIN(sdt::FoamDemo);
	// PUSH_MAIN(sdt::CellDemo);
	// PUSH_MAIN(sdt::LandScapeDemo);
	PUSH_MAIN(sdt::CharacterMoveDemo);
	// PUSH_MAIN(DelaunayTriangleDemo);
	// PUSH_MAIN(ImGuiDemo);
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

void Main::PreRender()
{
	for (IExecutable* executable : Executables)
		executable->PreRender();
}

void Main::Render()
{
	for (IExecutable* executable : Executables)
		executable->Render();
}

void Main::PostRender()
{
	for (IExecutable* executable : Executables)
		executable->PostRender();
}

void Main::Push(IExecutable* Executable)
{
	Executables.push_back(Executable);

	Executable->Initialize();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	const D3DDesc desc {
		L"DirectX11 Practice",
		hInstance,
		nullptr,
		1280,
		720,
		Color(0.3f, 0.3f, 0.3f, 1.0f)
	};
	D3D::SetDesc(desc);

	Main * main = new Main();
	const int wParam = static_cast<int>(Window::Run(main));

	SAFE_DELETE(main)

	return wParam;
}
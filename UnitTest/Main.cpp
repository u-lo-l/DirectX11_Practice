// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"

#include "Execute/028_InstancingDemo/InstancingDemo.h"
#include "Execute/DisplacementMappingDemo/BasicTessellationDemo.h"
#include "Execute/DisplacementMappingDemo/DisplacementMappingDemo.h"
#include "Execute/DisplacementMappingDemo/PNTriangleDemo.h"
#include "Execute/LandscapeDemo/CellDemo.h"
#include "Execute/LandscapeDemo/LandScapeDemo.h"
#include "Execute/LODDemo/TerrainTessellationDemo.h"
#include "Execute/OceanDemo/GaussianRandomDemo.h"
#include "Execute/OceanDemo/OceanDemo.h"
#include "Execute/OceanDemo/PhilipsSpectrumDemo.h"
#include "Execute/OceanDemo/TextureTransposeDemo.h"
#include "Systems/Window.h"
#include "Execute/Shadow/ShadowDemo.h"

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
	// PUSH_MAIN(sdt::LandScapeDemo);
	PUSH_MAIN(sdt::CellDemo);
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
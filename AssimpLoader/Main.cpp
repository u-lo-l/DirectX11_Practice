// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

#include "AssimpLoader/ExportFile.h"
#include "AssimpLoader/DrawModel.h"

void Main::Initialize()
{
	PUSH_MAIN(Sdt::ExportFile);
	PUSH_MAIN(Sdt::DrawModel);
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
	desc.AppName = L"D3D Assimp Loader";
	desc.Instance = hInstance;
	desc.Handle = nullptr;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.1f, 0.1f, 0.1f, 1.0f);

	D3D::SetDesc(desc);

	// ReSharper disable once CppUseAuto
	Main * main = new Main();
	const WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main)
	return static_cast<int>(wParam);
}
// ReSharper disable CppInconsistentNaming
#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"

#include "AssimpLoader/ExportFile.h"
#include "AssimpLoader/DrawModel.h"
#include "AssimpLoader/DrawCSkeletalMesh.h"

void Main::Initialize()
{
	PUSH_MAIN(sdt::ExportFile);
	// PUSH_MAIN(sdt::DrawModel);
	PUSH_MAIN(DrawCSkeletalMesh);
}

void Main::Destroy()
{
	for (IExecutable * executable : Executables)
	{
		SAFE_DELETE(executable)
	}
}

void Main::Tick()
{
	for (IExecutable * executable : Executables)
		executable->Tick();
}

void Main::Render()
{
	for (IExecutable * executable : Executables)
		executable->Render();
}

void Main::Push(IExecutable* Executable)
{
	ASSERT(!!Executable , String::Format("%s | Executable Not Valid", __FUNCTION__).c_str());

	if (Executable->IsTickable())
		Executables.push_back(Executable);
	Executable->Initialize();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	D3DDesc desc;
	desc.AppName = L"D3D Assimp Loader";
	desc.Instance = hInstance;
	desc.Handle = nullptr;
	desc.WindowWidth = 1280;
	desc.WindowHeight = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);

	D3D::SetDesc(desc);

	Main * main   = new Main();
	WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main)
	return static_cast<int>(wParam);
}
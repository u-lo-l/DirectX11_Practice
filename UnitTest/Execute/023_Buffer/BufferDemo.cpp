#include "Pch.h"
#include "BufferDemo.h"

namespace Sdt
{
	void BufferDemo::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		Landscape = new Terrain(L"11_Grid.fx", L"GrayScale02.png");
		MainCamera->SetPosition(15, 10, -30);
		MainCamera->SetRotation(0, 0, 0);
	}

	void BufferDemo::Destroy()
	{
		SAFE_DELETE(Landscape);
	}

	void BufferDemo::Tick()
	{
		Landscape->Tick();
	}

	void BufferDemo::Render()
	{
		Landscape->SetPass(1);
		Landscape->Render();
	}
}

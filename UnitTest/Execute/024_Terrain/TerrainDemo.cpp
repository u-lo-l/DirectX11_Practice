#include "Pch.h"
#include "TerrainDemo.h"

namespace Sdt
{
	void TerrainDemo::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		Landscape = new Terrain(L"17_TerrainNormal.fx", L"Terrain/Gray256.png");
		MainCamera->SetPosition(15, 10, -30);
		MainCamera->SetRotation(0, 0, 0);
	}

	void TerrainDemo::Destroy()
	{
		SAFE_DELETE(Landscape);
	}

	void TerrainDemo::Tick()
	{
		Landscape->Tick();
	}

	void TerrainDemo::Render()
	{
		Landscape->SetPass(0);
		Landscape->Render();
	}
}

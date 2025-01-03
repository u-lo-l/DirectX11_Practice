#include "Pch.h"
#include "TerrainDemo.h"

constexpr wchar_t BoxTextureRef[] = L"../../_Textures/Box.png";
constexpr wchar_t GrassTextureRef[] = L"../../_Textures/Forest Floor.jpg";
constexpr wchar_t WeightRef[] = L"../../_Textures/GrayScale.png";
constexpr wchar_t GrayRef[] = L"../../_Textures/GrayScale02.png";

namespace Sdt
{
	void TerrainDemo::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		Landscape = new Terrain(L"11_Grid.fx", L"GrayScale02.png");
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
		Landscape->SetPass(1);
		Landscape->Render();
	}
}

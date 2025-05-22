#include "Pch.h"
#include "CellDemo.h"

namespace sdt
{
	void CellDemo::Initialize()
	{
		const Vector TerrainExtent = {2560, 1024, 2560};
		const UINT GridSize = static_cast<UINT>(powf(2, 6));
		Sky = new SkySphere();
		Context::Get()->GetCamera()->SetPosition(-830, 1020, -441);
		Context::Get()->GetCamera()->SetRotation(17, 50, -0);

		LandScape_QuadTree::LandScapeDesc Desc =
		{
			TerrainExtent,
			256,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		const LandScape::LandScapeDesc LandscapeDesc
		{
			TerrainExtent,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		TerrainWidthCell = new LandScape_QuadTree(Desc);
		TerrainNoCell = new LandScape(LandscapeDesc);
	}

	void CellDemo::Tick()
	{
		Sky->Tick();
		if (bRenderCell)
			TerrainWidthCell->Tick();
		else
			TerrainNoCell->Tick();
	}

	void CellDemo::Render()
	{
		ImGui::Checkbox("Render Cell", &bRenderCell);
		Sky->Render();
		if (bRenderCell)
			TerrainWidthCell->Render(true);
		else
			TerrainNoCell->Render();
	}

	void CellDemo::Destroy()
	{
		SAFE_DELETE(TerrainWidthCell);
		SAFE_DELETE(TerrainNoCell);
		SAFE_DELETE(Sky);
	}
}

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

		LandScape::LandScapeDesc Desc =
		{
			TerrainExtent,
			256,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		TerrainWidthCell = new LandScape(Desc);
	}

	void CellDemo::Tick()
	{
		Sky->Tick();
		TerrainWidthCell->Tick();
	}

	void CellDemo::Render()
	{
		Sky->Render();
		TerrainWidthCell->Render(true);
	}

	void CellDemo::Destroy()
	{
		SAFE_DELETE(TerrainWidthCell);
		SAFE_DELETE(Sky);
	}
}

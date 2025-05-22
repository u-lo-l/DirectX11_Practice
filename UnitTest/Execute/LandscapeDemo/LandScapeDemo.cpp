#include "Pch.h"
#include "LandScapeDemo.h"

namespace sdt
{
	void LandScapeDemo::Initialize()
	{
		// constexpr float SeaLevel = 50.f;

		const Vector TerrainExtent = {2560, 1024, 2560};
		const UINT GridSize = static_cast<UINT>(powf(2, 4));
		const LandScape::LandScapeDesc LandscapeDesc =
		{
			TerrainExtent,
			256,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(LandscapeDesc);

		// TODO : QuadTree로 공간 관리. Foliage
		// Grasses = new Foliage(Terrain, SeaLevel + 2.f);

		Sky = new SkySphere(L"Environments/SkyDawn.dds", 0.5f);
	}

	void LandScapeDemo::Destroy()
	{
		SAFE_DELETE(Sky);
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Grasses);
	}

	void LandScapeDemo::Tick()
	{
		if (!!Sky)
			Sky->Tick();
		if (!!Terrain)
			Terrain->Tick();
		if (!!Grasses)
			Grasses->Tick();
	}

	void LandScapeDemo::Render()
	{
		if (!!Sky)
			Sky->Render();
		if (!!Terrain)
			Terrain->Render();
		if (!!Grasses)
			Grasses->Render();
	}
}

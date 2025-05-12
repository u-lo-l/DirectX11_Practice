#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		constexpr float SeaLevel = 50.f;
		const Vector2D SeaSize = {4096, 4096};
		
		// Terrain = new Terrain2(L"Terrain/Gray512.png", 64);
		// Terrain = new Terrain2(L"Terrain/Gray512.png", 64);
		// Terrain = new LandScape(L"Terrain/GrandMountain/Height Map PNG.png", 32);
		// const LandScape::LandScapeDesc LandscapeDesc
		// {
		// 	L"Terrain/GrandMountain/Height Map TIF.tif",
		// 	L"Terrain/GrandMountain/Bump Map TIF.tif",
		// 	{},
		// 	{}
		// 	// L"Terrain/GrandMountain/Bump Map PNG.png",
		// 	// L"Terrain/GrandMountain/Normal Map PNG.png",
		// };
		// Terrain = new LandScape(LandscapeDesc, 32);
		// Terrain->SetHeightScale(1000);

		// TODO : QuadTree로 공간 관리. Foliage
		// Grasses = new Foliage(Terrain, SeaLevel + 2.f);

		// Sky = new SkySphere(L"Environments/SunSetCube1024.dds", 0.5f);
		Sky = new SkySphere(L"Environments/GrassCube1024.dds", 0.5f);
		
		const Ocean::OceanDesc OceanDesc{
			static_cast<UINT>(SeaSize.X),
			static_cast<UINT>(SeaSize.Y),
			32,
			{0, 0},
			SeaLevel,
			Sky->GetTexture(),
			Terrain->GetHeightMap(),
			{0, 0},
			{static_cast<float>(Terrain->GetWidth()), static_cast<float>(Terrain->GetHeight()) },
			Terrain->GetHeightScale()
		};
		Sea = new Ocean(OceanDesc);

		if (!!Sea && !!Terrain)
		{
			const Vector2D TerrainCenter = {(float)Terrain->GetWidth() / 2, (float)Terrain->GetHeight() / 2};
			Sea->SetWorldPosition({TerrainCenter.X - SeaSize.X / 2, SeaLevel, TerrainCenter.Y - SeaSize.Y / 2});
		}
	}

	void OceanDemo::Destroy()
	{
		SAFE_DELETE(Sea);
		SAFE_DELETE(Sky);
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Grasses);
	}

	void OceanDemo::Tick()
	{
		if (!!Sky)
			Sky->Tick();
		if (!!Terrain)
			Terrain->Tick();
		if (!!Grasses)
			Grasses->Tick();
		if (!!Sea)
		{
			Sea->Tick();
			if (Keyboard::Get()->IsPressed(VK_SPACE))
			{
				Sea->SaveHeightMap();
			}
		}
	}

	void OceanDemo::Render()
	{
		if (!!Sky)
			Sky->Render();
		if (!!Terrain)
			Terrain->Render();
		if (!!Grasses)
			Grasses->Render();
		if (!!Sea)
			Sea->Render();
	}
}

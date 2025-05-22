#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		const Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 571,30,571);
		MainCamera->SetRotation( 0,60,0 );
		
		const Vector2D TerrainSize = Vector2D(1024, 1024) * 4 ;
		constexpr float TerrainHeight = 512 * 8;

		constexpr float SeaLevel = 10.f;
		const Vector2D SeaSize = Vector2D(1024, 1024) * 16;

		const LandScape::LandScapeDesc LandscapeDesc
		{
				{
					TerrainSize.X,
				   TerrainHeight,
				   TerrainSize.Y
				},
			128,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(LandscapeDesc);

		Sky = new SkySphere(L"Environments/SkyDawn.dds", 0.5f);
		const Ocean::OceanDesc OceanDesc{
			static_cast<UINT>(SeaSize.X),
			static_cast<UINT>(SeaSize.Y),
			512,
			128,
			{0, 0},
			SeaLevel,
			Sky->GetTexture(),
			nullptr,
			{0, 0},
			TerrainSize,
			TerrainHeight,
			Vector2D(-15, -20) * 5.f
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
			Sea->Tick();
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

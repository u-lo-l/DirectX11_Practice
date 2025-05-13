#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		const Vector2D TerrainSize = {1024, 1024};
		constexpr float TerrainHeight = 512;

		constexpr float SeaLevel = 4.f;
		const Vector2D SeaSize = {4096, 4096};
		
		const LandScape::LandScapeDesc LandscapeDesc
		{
			(UINT)TerrainSize.X,
			(UINT)TerrainSize.Y,
			TerrainHeight,
			32,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/UE_Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/UE_Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/UE_Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/UE_Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(LandscapeDesc);
		
		// TODO : QuadTree로 공간 관리. Foliage
		Grasses = new Foliage(Terrain, 12);

		Sky = new SkySphere(L"Environments/SkyDawn.dds", 0.5f);

		const Ocean::OceanDesc OceanDesc{
			static_cast<UINT>(SeaSize.X),
			static_cast<UINT>(SeaSize.Y),
			32,
			{0, 0},
			SeaLevel,
			Sky->GetTexture(),
			Terrain->GetHeightMap(),
			{0, 0},
			TerrainSize,
			Terrain->GetHeightScale(),
			{50.f, 30.f}
		};
		Sea = new Ocean(OceanDesc);

		if (!!Sea && !!Terrain)
		{
			const Vector2D TerrainCenter = {(float)Terrain->GetWidth() / 2, (float)Terrain->GetHeight() / 2};
			Sea->SetWorldPosition({TerrainCenter.X - SeaSize.X / 2, SeaLevel, TerrainCenter.Y - SeaSize.Y / 2});
		}

		const Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 470,21,172 );
		MainCamera->SetRotation( 0,90,0);
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

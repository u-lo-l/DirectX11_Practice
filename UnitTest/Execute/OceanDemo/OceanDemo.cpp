#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		const Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 571,30,571);
		MainCamera->SetRotation( 0,60,0 );
		
		constexpr float SeaLevel = 10.f;

		const Vector TerrainDimension = {2560, 1024, 2560};
		const UINT GridSize = static_cast<UINT>(powf(2, 6));
		const LandScape::LandScapeDesc LandscapeDesc =
		{
			TerrainDimension,
			128,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(LandscapeDesc);

		Sky = new SkySphere(L"Environments/SkyDawn.dds", 0.5f);
		Vector SeaExtent = Vector(1024, 1024, 1024) * 16.f;
		const Ocean::OceanDesc OceanDesc{
			SeaExtent,
			512,
			256,
			16,
			SeaLevel,
			Sky->GetTexture(),
			nullptr,
			{0, 0},
			{0, 0},
			0,
			{-15, -20}
		};
		Sea = new Ocean(OceanDesc);

		if (!!Sea && !!Terrain)
		{
			const Vector2D TerrainCenter = {Terrain->GetDimension().X * 0.5f, Terrain->GetDimension().Z * 0.5f};
			Sea->SetWorldPosition({TerrainCenter.X - SeaExtent.X * 0.5f, SeaLevel, TerrainCenter.Y - SeaExtent.Z * 0.5f});
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

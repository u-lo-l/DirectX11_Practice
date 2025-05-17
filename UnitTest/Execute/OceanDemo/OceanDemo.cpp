#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		const Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( -1300,1900,2200 );
		MainCamera->SetRotation( 10,60,0);
		
		const Vector2D TerrainSize = {1024, 1024} ;
		constexpr float TerrainHeight = 512;

		constexpr float SeaLevel = 4.f;
		const Vector2D SeaSize = Vector2D(1024, 1024) * 16;

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
			{0, 0},
			0,
			{-15, -20}
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

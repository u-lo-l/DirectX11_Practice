#include "Pch.h"
#include "TerrainTessellationDemo.h"

namespace sdt
{
	void TerrainTessellationDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 420, 41, 304 );
		MainCamera->SetRotation( 7,1,0);

		Terrain = new LandScape(L"Terrain/Gray512.png", 32);
		// Terrain = new Terrain2(L"Terrain/Terrain2.jpg", 32);
		Grasses = new Foliage(Terrain);
	}

	void TerrainTessellationDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Grasses);
	}

	void TerrainTessellationDemo::Tick()
	{
		Terrain->Tick();
		Grasses->Tick();
	}

	void TerrainTessellationDemo::Render()
	{
		Terrain->Render();
		Grasses->Render();
	}
}

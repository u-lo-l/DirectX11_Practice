#include "Pch.h"
#include "CellDemo.h"

namespace sdt
{
	void CellDemo::Initialize()
	{
		LandScape_QuadTree::LandScapeDesc Desc =
		{
			Vector(2048, );
		};
		Terrain = new LandScape_QuadTree(Desc);
	}

	void CellDemo::Tick()
	{
		Terrain->Tick();
	}

	void CellDemo::Render()
	{
		Terrain->Render(true);
	}

	void CellDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
	}
}

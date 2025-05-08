#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		Sea = new Ocean(32);
	}

	void OceanDemo::Destroy()
	{
		SAFE_DELETE(Sea);
	}

	void OceanDemo::Tick()
	{
		Sea->Tick();
		if (Keyboard::Get()->IsPressed(VK_SPACE))
		{
			Sea->SaveHeightMap();
		}
	}

	void OceanDemo::Render()
	{
		Sea->Render();
	}
}

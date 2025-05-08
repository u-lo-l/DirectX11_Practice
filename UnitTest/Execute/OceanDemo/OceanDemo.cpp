#include "Pch.h"
#include "OceanDemo.h"

namespace sdt
{
	void OceanDemo::Initialize()
	{
		Sea = new Ocean(2);
	}

	void OceanDemo::Destroy()
	{
		SAFE_DELETE(Sea);
	}

	void OceanDemo::Tick()
	{
		Sea->Tick();
	}

	void OceanDemo::Render()
	{
		Sea->Render();
	}
}

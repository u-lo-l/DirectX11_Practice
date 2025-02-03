#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		Context::Get()->GetCamera()->SetRotation(12, 0, 0);
		Context::Get()->GetCamera()->SetPosition(35, 10, 55);
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Drawer);
	}

	void InstancingDemo::Tick()
	{

	}

	void InstancingDemo::Render()
	{

	}
}

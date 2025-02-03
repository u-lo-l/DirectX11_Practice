#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		Context::Get()->GetCamera()->SetRotation(12, 0, 0);
		Context::Get()->GetCamera()->SetPosition(0, 100, -200);

		Plane = new Model(L"Plane");
		Plane->GetWorldTransform()->SetScale({10,3,1});
		Plane->GetWorldTransform()->SetRotation({0,90,0});

		Cube = new Model(L"Cube");
		Cube->GetWorldTransform()->SetScale({0.5f,0.5f,0.5f});
		Cube->GetWorldTransform()->SetPosition({0, 25, 0});

		Adam1 = new Model(L"Adam");
		Adam1->GetWorldTransform()->SetPosition({100, 0, 0});
		Adam2 = new Model(L"Adam");
		Adam2->GetWorldTransform()->SetPosition({-100, 0, 0});
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		SAFE_DELETE(Drawer);
	}

	void InstancingDemo::Tick()
	{
		Plane->Tick();
		Cube->Tick();
		Adam1->Tick();
		Adam2->Tick();
	}

	void InstancingDemo::Render()
	{
		Plane->Render();
		Cube->Render();
		Adam1->Render();
		Adam2->Render();
	}
}

#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		Context::Get()->GetCamera()->SetRotation(12, 0, 0);
		Context::Get()->GetCamera()->SetPosition(0, 100, -200);

		Plane = new Model(L"Plane");
		Plane->GetWorldTransform()->SetScale({30,3,1});
		Plane->GetWorldTransform()->SetRotation({0,90,0});

		Cube = new Model(L"Cube");
		Cube->GetWorldTransform()->SetScale({0.1f,0.1f,0.1f});
		Cube->GetWorldTransform()->SetPosition({0, 5, 0});

		Adam = new Model(L"Adam");
		Adam->GetWorldTransform()->SetScale({0.5f,0.5f,0.5f});
		Adam->GetWorldTransform()->SetPosition({100, 0, 0});

		for (float x = 0.0f; x <= 100.f ; x += 10.0f)
		{
			Positions.push_back({x, 0.5f, -5.0f});
		}
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		SAFE_DELETE(Cube);
		SAFE_DELETE(Adam);
		SAFE_DELETE(Drawer);
	}

	void InstancingDemo::Tick()
	{
		Plane->Tick();
		Cube->Tick();
		Adam->Tick();
	}

	void InstancingDemo::Render()
	{
		Plane->Render();
		for (UINT i = 0 ; i < Positions.size(); i++)
		{
			const Vector & v = Positions[i];

			Cube->GetWorldTransform()->SetPosition(v);
			Cube->Render();
			Vector v2 = {v.X, v.Y, v.Z + 5.f};
			Adam->GetWorldTransform()->SetPosition(v2);
			Adam->Render();
		}
	}
}

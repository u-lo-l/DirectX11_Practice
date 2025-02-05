#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		Context::Get()->GetCamera()->SetRotation(12, 0, 0);
		Context::Get()->GetCamera()->SetPosition(0, 0	, 0);

		Transform * tf = nullptr;
		// Plane = new Model(L"Plane");
		// tf = Plane->AddTransforms();
		// tf->SetScale({10,1,10});
		// tf->SetRotation({0,90,0});
		// tf->Tick();
		
		Cube = new Model(L"Cube");
		Sphere = new Model(L"Sphere");
		
		for (int x = -500 ; x <= 500; x += 10.f)
		{
			Vector Pos {(float)x, 0.5f, -10.f };
			tf = Cube->AddTransforms();
			tf->SetPosition(Pos);
			tf->SetScale({0.075f,0.075f,0.075f});
			tf->SetRotation({0, Math::Random(-180.f, 180.f), 0});
			tf->Tick();
		
			Pos.Z = 30.f;
			tf = Sphere->AddTransforms();
			tf->SetScale({0.075f,0.075f,0.075f});
			tf->SetPosition(Pos);
			tf->Tick();

		}
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Sphere);
		SAFE_DELETE(Cube);
		SAFE_DELETE(Plane);
		SAFE_DELETE(Drawer);
	}

	void InstancingDemo::Tick()
	{
		// Plane->Tick();
		Cube->Tick();
		Sphere->Tick();
	}

	void InstancingDemo::Render()
	{
		// Plane->Render();
		Cube->Render();
		Sphere->Render();
	}
}

#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		// Context::Get()->GetCamera()->SetRotation(-10, 0, 0);
		Context::Get()->GetCamera()->SetPosition(0, 150, -250);

		Transform * tf = nullptr;
		// Plane = new Model(L"Plane");
		// tf = Plane->AddTransforms();
		// tf->SetScale({10,1,10});
		// tf->SetRotation({0,90,0});
		// tf->Tick();
		
		Airplane = new Model(L"Airplane");
		Sphere = new Model(L"Sphere");
		
		for (int x = -250 ; x <= 250; x += 25.f)
		{
			Vector Pos {static_cast<float>(x), 0.5f, -10.f };
			tf = Airplane->AddTransforms();
			tf->SetPosition(Pos);
			tf->SetScale({0.005f,0.005f,0.005f});
			tf->SetRotation({0, Math::Random(-180.f, 180.f), 0});
			tf->Tick();
		
			Pos.Z = 30.f;
			tf = Sphere->AddTransforms();
			tf->SetScale({0.075f,0.075f,0.075f});
			tf->SetPosition(Pos);
			tf->Tick();
		}

		Models.insert(Models.end(), {
			new Model(L"Mousey"),
			new Model(L"Cube"),
			new Model(L"Airplane"),
			new Model(L"Sphere"),
		});
		Scales.insert(Scales.end(), {
			{0.05f,0.05f,0.05f},
			{0.025f,0.025f,0.025f},
			{0.0025f,0.0025f,0.0025f},
			{0.075f,0.075f,0.075f},
		});
		PosZ.insert(PosZ.end(), {-30, -15, 0, 15} );

		const int ModelCount = Models.size();
		for (int x = -500 ; x <= 500; x += 40.f)
		{
			Vector Pos {static_cast<float>(x), 0, 0 };
			for (int m = 0; m < ModelCount; m++)
			{
				tf = Models[m]->AddTransforms();
				tf->SetScale(Scales[m]);
				Pos.Z = PosZ[m];
				tf->SetPosition(Pos);
				tf->SetRotation({Math::Random(-180.f, 180.f), Math::Random(-180.f, 180.f), Math::Random(-180.f, 180.f)});
				tf->Tick();
			}
		}
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Sphere);
		SAFE_DELETE(Airplane);
		SAFE_DELETE(Plane);
		SAFE_DELETE(Drawer);
	}

	void InstancingDemo::Tick()
	{
		// Plane->Tick();
		for ( Model * m : Models)
			m->Tick();
	}

	void InstancingDemo::Render()
	{
		// Plane->Render();
		for ( Model * m : Models)
			m->Render();
	}
}

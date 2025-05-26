#include "Pch.h"
#include "CharacterMoveDemo.h"

namespace sdt
{
	
	void CharacterMoveDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 0, 10, -50 );

		SetTerrain();
		SetCharacter();
	}

	void CharacterMoveDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Character);
	}

	void CharacterMoveDemo::Tick()
	{
		const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
		Vector DeltaPosition = {0, 0, 0};

		if (sdt::Mouse::Get()->IsPress(MouseButton::Left) == true)
		{
			if (Keyboard::Get()->IsPressed('W') == true)
			{
				DeltaPosition += Vector::Forward * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('S') == true)
			{
				DeltaPosition -= Vector::Forward * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('D') == true)
			{
				DeltaPosition += Vector::Right * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('A') == true)
			{
				DeltaPosition -= Vector::Right * DeltaTime * 20;
			}
			Character->GetTransform(0)->AddWorldTranslation(DeltaPosition);
		}

		if (!!Terrain) Terrain->Tick();
		if (!!Character) Character->Tick();
	}

	void CharacterMoveDemo::PreRender()
	{
		
	}

	void CharacterMoveDemo::Render()
	{
		if (!!Terrain) Terrain->Render();
		if (!!Character) Character->Render();
	}

	void CharacterMoveDemo::PostRender()
	{

	}

	void CharacterMoveDemo::SetTerrain()
	{
		LandScape::LandScapeDesc Desc =
		{
			Vector(512, 128, 512),
			512,
			64,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(Desc);
	}

	void CharacterMoveDemo::SetCharacter()
	{
		Character = new Model(L"Adam");
		Transform * tf = Character->AddTransforms();
		tf->SetWorldPosition({0,0,0});
		tf->SetScale({0.01f,0.01f,0.01f});
		tf->SetWorldRotation({0, 180 * Math::DegToRadian, 0});
	}
}

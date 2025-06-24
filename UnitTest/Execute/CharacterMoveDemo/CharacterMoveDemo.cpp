#include "Pch.h"
#include "CharacterMoveDemo.h"

#include "Model/Character.h"
#include "Systems/RenderManager.h"

namespace sdt
{
	
	void CharacterMoveDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 0, 10, -50 );

		// SetTerrain();
		SetCharacter();
	}

	void CharacterMoveDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Adam);
	}

	void CharacterMoveDemo::Tick()
	{
		const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
		Vector MoveDirection = {0, 0, 0};
		float Speed = 1.f;
		if (Mouse::Get()->IsPress(MouseButton::Left) == true)
		{
			if (Keyboard::IsPressed('W') == true)
			{
				MoveDirection += Vector::Forward;
			}
			if (Keyboard::IsPressed('S') == true)
			{
				MoveDirection -= Vector::Forward;
			}
			if (Keyboard::IsPressed('D') == true)
			{
				MoveDirection += Vector::Right;
			}
			if (Keyboard::IsPressed('A') == true)
			{
				MoveDirection -= Vector::Right;
			}
			if (Keyboard::IsPressed(VK_CONTROL) == true)
			{
				Speed = 4.f;
			}
			MoveDirection.Normalize();
		}
		if (!!Terrain)
		{
			Terrain->Tick();
		}
		if (!!Adam)
		{
			ImGui::Begin("Adam Tf Info");
			Transform * const AdamTf = Adam->GetTransform();
			const Vector & Location = AdamTf->GetWorldPosition();
			const Vector & Forward = AdamTf->GetForward();
			const Vector & Right = AdamTf->GetRight();
			ImGui::TextColored({0, 255, 0, 255}, "P : %.3f, %.3f, %.3f", Location.X, Location.Y, Location.Z);
			ImGui::TextColored({0, 255, 0, 255}, "F : %.3f, %.3f, %.3f", Forward.X, Forward.Y, Forward.Z);
			ImGui::TextColored({0, 255, 0, 255}, "R : %.3f, %.3f, %.3f", Right.X, Right.Y, Right.Z);
			ImGui::End();
			const Vector Velocity = MoveDirection * Speed * DeltaTime * 20;
			AdamTf->AddLocalTranslation(Velocity);
			Adam->Tick();
		}
	}

	void CharacterMoveDemo::PreRender()
	{
		
	}

	void CharacterMoveDemo::Render()
	{
		if (!!Terrain) Terrain->Render();
		if (!!Adam) Adam->Render();
	}

	void CharacterMoveDemo::PostRender()
	{

	}

	void CharacterMoveDemo::SetTerrain()
	{
		LandScape::LandScapeDesc Desc =
		{
			Vector(2048, 2048, 2048),
			256,
			64,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(Desc);
	}

	void CharacterMoveDemo::SetCharacter()
	{
		Adam = new Character();
		Adam->SetSkeletalMesh(L"Adam");
		AnimationClip * Idle = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Stop", true);
		AnimationClip * Walk_F = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Walk_F", true);
		AnimationClip * Walk_B = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Walk_B", true);
		AnimationClip * Walk_R = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Walk_R", true);
		AnimationClip * Walk_L = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Walk_L", true);
		AnimationClip * Run_F = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Run_F", true);
		AnimationClip * Run_B = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Run_B", true);
		AnimationClip * Run_R = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Run_R", true);
		AnimationClip * Run_L = new AnimationClip(Adam->GetSkeleton(), L"Adam/Locomotion/Run_L", true);
	
		AnimationBlendSpace2D * BS_Locomotion = new AnimationBlendSpace2D(
			Adam->GetSkeleton(),
			"BS_AdamLocomotion"
		);
		BS_Locomotion->SetHorizontalRange(-4,4);
		BS_Locomotion->SetVerticalRange(-4,4);
		BS_Locomotion->SetHorizontalWrapped(false);
		BS_Locomotion->SetVerticalWrapped(false);
		BS_Locomotion->AddAnimation(Idle  , { 0,  0});
		BS_Locomotion->AddAnimation(Walk_F, { 0,  1});
		BS_Locomotion->AddAnimation(Walk_B, { 0, -1});
		BS_Locomotion->AddAnimation(Walk_R, { 1,  0});
		BS_Locomotion->AddAnimation(Walk_L, {-1,  0});
		BS_Locomotion->AddAnimation(Run_F , { 0,  4});
		BS_Locomotion->AddAnimation(Run_B , { 0, -4});
		BS_Locomotion->AddAnimation(Run_R , { 4,  0});
		BS_Locomotion->AddAnimation(Run_L , {-4,  0});
		BS_Locomotion->EndAddingAnimation();
		Adam->GetAnimationController()->SetCurrentBlendSpace(BS_Locomotion);
	}
}

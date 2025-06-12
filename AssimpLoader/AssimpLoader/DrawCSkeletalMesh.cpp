#include "Pch.h"
#include "DrawCSkeletalMesh.h"

#include "Model/Character.h"
#include "Model/Animation/AnimationClip.h"

void DrawCSkeletalMesh::Initialize()
{
	Context::Get()->GetCamera()->SetPosition(0, 10, -20);
	// Context::Get()->GetCamera()->SetRotation({0, Math::Pi, 0});
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
	AnimationClip * RollFront = new AnimationClip(Adam->GetSkeleton(), L"Adam/RollFront", true);
	

	{
		// Adam->AddAnimationClip("RollFront", RollFront);
		// Adam->AddAnimationClip("Walk", Walk);
	}

	{
		// AnimationBlendSpace1D * BS_Locomotion = new AnimationBlendSpace1D(Adam->GetSkeleton());
		// BS_Locomotion->SetHorizontalRange(0.f, 4.f);
		// BS_Locomotion->AddAnimation(Idle, 0.f);
		// BS_Locomotion->AddAnimation(Walk, 1.f);
		// BS_Locomotion->AddAnimation(Run, 4.f);
		// BS_Locomotion->SetWrapped(false);
		// Adam->GetAnimationController()->SetCurrentBlendSpace(BS_Locomotion);
	}

	{
		AnimationBlendSpace2D * BS_Locomotion = new AnimationBlendSpace2D(Adam->GetSkeleton());
		BS_Locomotion->SetHorizontalRange(-4,4);
		BS_Locomotion->SetVerticalRange(-4,4);
		BS_Locomotion->SetHorizontalWrapped(false);
		BS_Locomotion->SetVerticalWrapped(false);
		BS_Locomotion->AddAnimation(Idle  , { 0,  0});
		// BS_Locomotion->AddAnimation(Walk_F, { 0,  1});
		// BS_Locomotion->AddAnimation(Walk_B, { 0, -1});
		BS_Locomotion->AddAnimation(Walk_R, { 1,  0});
		BS_Locomotion->AddAnimation(Walk_L, {-1,  0});
		// BS_Locomotion->AddAnimation(Run_F , { 0,  4});
		// BS_Locomotion->AddAnimation(Run_B , { 0, -4});
		BS_Locomotion->AddAnimation(Run_R , { 4,  0});
		BS_Locomotion->AddAnimation(Run_L , {-4,  0});
		BS_Locomotion->EndAddingAnimation();
		Adam->GetAnimationController()->SetCurrentBlendSpace(BS_Locomotion);
	}
}

void DrawCSkeletalMesh::Destroy()
{
	SAFE_DELETE(Adam);
}

void DrawCSkeletalMesh::Tick()
{
	Adam->Tick();
}

void DrawCSkeletalMesh::Render()
{
	Adam->Render();
}

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
	AnimationClip * Idle = new AnimationClip(Adam->GetSkeleton(), L"Adam/Stop", true);
	AnimationClip * Walk = new AnimationClip(Adam->GetSkeleton(), L"Adam/Walk_F", true);
	AnimationClip * Run = new AnimationClip(Adam->GetSkeleton(), L"Adam/Run_F", true);
	Idle->SetLoop(true);
	// Adam->AddAnimationClip("Idle", Idle);
	// Adam->AddAnimationClip("Walk", Walk);
	AnimationBlendSpace1D * BS_Locomotion = new AnimationBlendSpace1D(Adam->GetSkeleton());
	BS_Locomotion->SetHorizontalRange(0.f, 4.f);
	BS_Locomotion->AddAnimation(Idle, 0.f);
	BS_Locomotion->AddAnimation(Walk, 1.f);
	BS_Locomotion->AddAnimation(Run, 4.f);
	BS_Locomotion->SetWrapped(false);
	Adam->GetAnimationController()->SetCurrentBlendSpace(BS_Locomotion);
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

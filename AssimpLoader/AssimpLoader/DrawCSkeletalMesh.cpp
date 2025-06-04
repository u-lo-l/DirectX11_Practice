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
	AnimationClip * Idle = new AnimationClip(Adam->GetSkeleton(), L"Adam/Idle");
	Idle->SetLoop(true);
	Adam->AddAnimationClip("Idle", Idle);
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

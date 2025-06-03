#include "Pch.h"
#include "DrawCSkeletalMesh.h"

#include "Model/Character.h"
#include "Model/Mesh/CSkeletalMesh.h"


void DrawCSkeletalMesh::Initialize()
{
	Context::Get()->GetCamera()->SetPosition(0, 10, -20);
	// Context::Get()->GetCamera()->SetRotation({0, Math::Pi, 0});
	Adam = new Character();
	Adam->SetSkeletalMesh(L"Adam");
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

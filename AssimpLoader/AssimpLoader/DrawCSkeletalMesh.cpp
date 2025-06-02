#include "Pch.h"
#include "DrawCSkeletalMesh.h"
#include "Model/Mesh/CSkeletalMesh.h"


void DrawCSkeletalMesh::Initialize()
{
	Context::Get()->GetCamera()->SetPosition(0, 10, 20);
	Context::Get()->GetCamera()->SetRotation({0, Math::Pi, 0});
	Mesh = new CSkeletalMesh(L"Adam");
}

void DrawCSkeletalMesh::Destroy()
{
	SAFE_DELETE(Mesh);
}

void DrawCSkeletalMesh::Tick()
{
	Mesh->Tick();
}

void DrawCSkeletalMesh::Render()
{
	Mesh->Render();
}

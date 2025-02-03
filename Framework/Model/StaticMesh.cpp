#include "framework.h"
#include "StaticMesh.h"

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::Tick()
{
	ModelMesh::Tick();
}

void StaticMesh::Render()
{
	ModelMesh::Render();
}

void StaticMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
}

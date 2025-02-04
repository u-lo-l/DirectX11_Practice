#include "framework.h"
#include "StaticMesh.h"

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::Tick(const ModelAnimation * CurrentAnimation)
{
	ModelMesh::Tick(CurrentAnimation);
}

void StaticMesh::Render()
{
	ModelMesh::Render();
}

void StaticMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
}

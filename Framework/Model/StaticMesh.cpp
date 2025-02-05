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

void StaticMesh::Render(bool bInstancing)
{
	ModelMesh::Render(bInstancing);
}

void StaticMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
}

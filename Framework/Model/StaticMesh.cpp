#include "framework.h"
#include "StaticMesh.h"

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::Tick( UINT InInstanceSize, const vector<ModelAnimation *> & InAnimations )
{
	ModelMesh::Tick(InInstanceSize, InAnimations);
}

// void StaticMesh::Tick(const ModelAnimation * CurrentAnimation)
// {
// 	ModelMesh::Tick(CurrentAnimation);
// }

void StaticMesh::Render(bool bInstancing)
{
	ModelMesh::Render(bInstancing);
}

void StaticMesh::CreateBuffers()
{
	ModelMesh::CreateBuffers();
}

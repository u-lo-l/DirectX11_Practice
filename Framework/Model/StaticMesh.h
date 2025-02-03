#pragma once
#include "ModelMesh.h"

class StaticMesh : public ModelMesh
{
public:
	StaticMesh();
	friend class Model;

private:
	~StaticMesh() override;
	void Tick() override;
	void Render() override;
	void CreateBuffers() override;
};

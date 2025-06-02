#pragma once
#include "SubMesh.h"

class StaticMesh : public SubMesh
{
public:
	StaticMesh();
	friend class Model;

private:
	~StaticMesh() override;
};

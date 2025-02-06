#pragma once
#include "ModelMesh.h"

class StaticMesh : public ModelMesh
{
public:
	StaticMesh();
	friend class Model;

private:
	~StaticMesh() override;
	// void Tick(const ModelAnimation * CurrentAnimation = nullptr) override;
	void Tick(UINT InInstanceSize, const vector<ModelAnimation *> & InAnimations) override;
	void Render(bool bInstancing = false) override;
	void CreateBuffers() override;
};

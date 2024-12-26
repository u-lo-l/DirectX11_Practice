#pragma once

#include "Renders/VertexLayout.h"
#include "Systems/IExecutable.h"

class Line2 : public IExecutable
{
public:
	typedef typename VertexColor InnerVertex;
public:
	void Initialize();
	void Destroy();

	void Tick();
	void Render();

private:
	Shader* shader = nullptr;

	vector<VertexColor> Vertices;
	ID3D11Buffer* VertexBuffer;
};

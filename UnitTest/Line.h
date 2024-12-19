#pragma once

#include "Systems/IExecutable.h"

class Line : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick();
	void Render();

private:
	Shader* shader;
	
	Vector Vertices[2];
	ID3D11Buffer* VertexBuffer;
};
#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

class Line : public IExecutable
{
public:
	void Initialize();
	void Destroy();

	void Tick();
	void Render();

private:
	Shader* shader;
	HlslShader<Vertex> * Drawer;
	
	Vertex Vertices[2];
	ID3D11Buffer* VertexBuffer = nullptr;
};
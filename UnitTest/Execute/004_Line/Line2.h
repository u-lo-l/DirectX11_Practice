#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

class Line2 : public IExecutable
{
public:
	using VertexType = VertexColor;
public:
	void Initialize();
	void Destroy();

	void Tick();
	void Render();

private:
	Shader* shader = nullptr;
	HlslShader<VertexType> * hlslShader = nullptr;

	vector<VertexColor> Vertices;
	ID3D11Buffer* VertexBuffer;
};

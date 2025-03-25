#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

class Line_Circle : public IExecutable
{
public:
	using VertexType = VertexColor;

public:
	void Initialize() override;
	void Destroy() override;

	void Tick() override;
	void Render() override;

private:
	Shader* shader = nullptr;
	HlslShader<VertexType> * hlslShader;

	vector<VertexType> Vertices;
	ID3D11Buffer * VertexBuffer = nullptr;
};

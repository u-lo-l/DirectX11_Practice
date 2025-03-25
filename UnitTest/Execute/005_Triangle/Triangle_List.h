#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

class Triangle_List : public IExecutable
{
public:
	using VertexType = VertexColor;

public:
	void Initialize() override;
	void Destroy() override;

	void Tick() override;
	void Render() override;

private:
	// Shader* shader = nullptr;
	HlslShader<VertexType> * Drawer = nullptr;

	vector<VertexType> Vertices;
	ID3D11Buffer * VertexBuffer = nullptr;
	ID3D11Buffer * VertexBuffer2 = nullptr;
};

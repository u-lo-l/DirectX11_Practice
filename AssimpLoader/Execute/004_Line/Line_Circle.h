#pragma once

#include "Systems/IExecutable.h"

class Line_Circle : public IExecutable
{
public:
	using InnerVertexType = VertexColor;

public:
	void Initialize() override;
	void Destroy() override;

	void Tick() override;
	void Render() override;

private:
	Shader* shader = nullptr;

	vector<InnerVertexType> Vertices;
	ID3D11Buffer * VertexBuffer = nullptr;
};

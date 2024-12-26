#pragma once

#include "Systems/IExecutable.h"

class Triangle_List : public IExecutable
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
	ID3D11Buffer * VertexBuffer2 = nullptr;
};

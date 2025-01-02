#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class TerrainDemo final : public IExecutable
	{
	public:
		using VertexType = VertexColor;
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;
	private:
		Terrain * Landscape = nullptr;

	// for drawing normal vectors
		Shader * Drawer = nullptr;
		Matrix WorldMatrix;
		VertexBuffer * VBuffer = nullptr;
		IndexBuffer * IBuffer = nullptr;
		vector<VertexColor> Vertice;
	};
}

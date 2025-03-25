#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

namespace Sdt
{
	class WorldDemo final : public IExecutable
	{
	private:
		static const int VERTEX_COUNT;
	public:
		using VertexType = Vertex;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		// Shader * shader = nullptr;
		HlslShader<VertexType> * Drawer = nullptr;

		VertexType * Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ViewMat;
		Matrix ProjectionMat;

		ID3D11Buffer* CB_World = nullptr;
		ID3D11Buffer* CB_View = nullptr;
		ID3D11Buffer* CB_Projection = nullptr;
	};
}

#pragma once

#include "Systems/IExecutable.h"
#include "Renders/Shader/HlslShader.hpp"

namespace Sdt
{
	class Grid final : public IExecutable
	{
	private :
		static UINT INDEX_PER_GRID;
		UINT Pass = 1;
		UINT Width = 4;
		UINT Height = 4;
	public:
		using VertexType = Vertex;
		using IndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		// Shader* shader = nullptr;
		HlslShader<VertexType> * Drawer = nullptr;

		UINT VertexCount = 0;
		VertexType* Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;

		UINT IndexCount = 0;
		UINT * Indices = nullptr;
		ID3D11Buffer* IndexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ViewMat;
		Matrix ProjectionMat;

		// World-View-Projection Constant Buffer
		ID3D11Buffer * WVPCBuffer = nullptr;
	};
}

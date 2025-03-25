#pragma once

#include "Systems/IExecutable.h"
#include "Renders/HlslShader.hpp"

namespace Sdt
{
	class WorldDemo2 final : public IExecutable
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
		HlslShader<VertexType> * Drawer;
		
		VertexType * Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;

		Matrix WorldMat[3];
		Matrix ViewMat;
		Matrix ProjectionMat;

		struct CBufferData
		{
			Matrix View;
			Matrix Projection;
		};
		struct CBufferData2
		{
			Matrix World;
			float Index;
			float Padding[3];
		};
		ID3D11Buffer* CBuffer1 = nullptr;
		ID3D11Buffer* CBuffer2 = nullptr;
	};
}

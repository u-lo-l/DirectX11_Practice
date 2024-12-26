#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class IndexBufferDemo final : public IExecutable
	{
	private:
		static const int VERTEX_COUNT;
		static const int INDEX_COUNT;

	public:
		using InnerVertexType = Vertex;
		using InnerIndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader * shader = nullptr;

		InnerVertexType * Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;
		UINT * Indices = nullptr;
		ID3D11Buffer * IndexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ViewMat;
		Matrix ProjectionMat;
	};
}

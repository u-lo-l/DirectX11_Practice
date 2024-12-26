#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class MouseDemo final : public IExecutable
	{
	private :
		static UINT INDEX_PER_GRID;
		UINT Pass = 1;
		UINT Width = 256;
		UINT Height = 256;
		Vector CamPosition;
	public:
		using InnerVertexType = Vertex;
		using InnerIndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader* shader = nullptr;

		UINT VertexCount = 0;
		InnerVertexType* Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;

		UINT IndexCount = 0;
		UINT * Indices = nullptr;
		ID3D11Buffer* IndexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ViewMat;
		Matrix ProjectionMat;
	private:
	};
}

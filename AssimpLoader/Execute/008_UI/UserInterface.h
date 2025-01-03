#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class UserInterface final : public IExecutable
	{
	private:
		static const int VERTEX_COUNT;

	public:
		using InnerVertexType = Vertex;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader * shader = nullptr;

		InnerVertexType * Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;
	};
}

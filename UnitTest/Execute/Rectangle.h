#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class Rectangle final : public IExecutable
	{
	private:
		static const int VERTEX_COUNT;
	public:
		using InnerVertexType = Vector;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader* shader = nullptr;

		//vector<InnerVertexType> Vertices;
		InnerVertexType * Vertices;
		ID3D11Buffer* VertexBuffer = nullptr;
	};
}

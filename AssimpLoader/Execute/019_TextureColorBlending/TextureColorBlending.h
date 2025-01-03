#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class TextureColorBlending final : public IExecutable
	{
	private:
		Camera* MainCamera = nullptr;

	public:
		using InnerVertexType = VertexTextureColor;
		using InnerIndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader* shader = nullptr;

		float LerpRate = 0;


		UINT VertexCount = 4;
		InnerVertexType* Vertices = nullptr;
		ID3D11Buffer* VertexBuffer = nullptr;

		UINT IndexCount = 6;
		UINT* Indices = nullptr;
		ID3D11Buffer* IndexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ProjectionMat;

		ID3D11ShaderResourceView * Srv = nullptr;
	};
}

#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class TextureDemo final : public IExecutable
	{
	private:
		Camera* MainCamera = nullptr;
	public:
		using InnerVertexType = VertexTexture;
		using InnerIndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		Shader* shader = nullptr;

		UINT Pass = 0;
		
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

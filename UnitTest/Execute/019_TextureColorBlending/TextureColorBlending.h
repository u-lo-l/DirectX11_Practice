#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class TextureColorBlending final : public IExecutable
	{
	private:
		Camera * MainCamera = nullptr;

	public:
		using VertexType = VertexTextureColor;
		using IndexType = UINT;

	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;

	private:
		// Shader* shader = nullptr;
		HlslShader<VertexType> * Drawer = nullptr;

		float LerpRate = 0;
		
		const static UINT VertexCount;
		vector<VertexType> Vertices;
		ID3D11Buffer* VertexBuffer = nullptr;

		const static UINT IndexCount;
		vector<UINT> Indices;
		ID3D11Buffer* IndexBuffer = nullptr;

		Matrix WorldMat;
		Matrix ViewMat;
		Matrix ProjectionMat;

		Texture * SampleTexture = nullptr;
		ID3D11SamplerState * SamplerState = nullptr;
		// World-View-Projection Constant Buffer
		ID3D11Buffer * WVPCBuffer = nullptr;
		ID3D11Buffer * LerpRateCBuffer = nullptr;
	};
}

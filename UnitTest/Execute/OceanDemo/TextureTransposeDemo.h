#pragma once
#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class TextureTransposeDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;
	private:
		// Texture * Original = nullptr;
		// RWTexture2D * Transposed = nullptr;
		// Hlsl2DTextureShader * OriginalShader = nullptr;
		// Hlsl2DTextureShader * TransposedShader = nullptr;
		// HlslComputeShader * TransposeComputeShader = nullptr;
		// RawBuffer * TransposeBuffer = nullptr;

		struct ConstDesc
		{
			UINT Width;
			UINT Height;
			UINT Padding[2];
		} ConstData = {};
		Texture * InputTexture = nullptr;
		RWTexture2D * TestOutTexture = nullptr;
		HlslComputeShader * TestComputeShader = nullptr;
		ConstantBuffer * CBuffer = nullptr;
	};
}
#pragma once
#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class PhilipsSpectrumDemo : public IExecutable
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

		void SetupShaders();
		void SetupResources();
		void GenerateGaussianRandoms();
		void GenerateInitialSpectrum() const;
		void UpdateSpectrum() const;
		void GetHeightMap() const;
		// struct IFFTSizeDesc
		// {
		// 	UINT Width;
		// 	UINT Height;
		// 	UINT Padding[2];
		// } IFFTSizeData = {};

		struct PhillipsInitDesc
		{
			float Width;
			float Height;
			Vector2D Wind = {1, 1};
		} PhillipsInitData = {};

		struct PhilipsUpdateDesc
		{
			float Width;
			float Height;
			float RunningTime;
			float InitTime;
		} PhilipsUpdateData = {};

		// Resources
		Texture * GaussianRandomTexture2D = nullptr;
		RWTexture2D * InitialSpectrumTexture2D = nullptr;		// H_init
		RWTexture2D * SpectrumTexture2D = nullptr;				// H_t
		RWTexture2D * IFFT_Row = nullptr;	// H_t -> IFFT(x)
		RWTexture2D * IFFT_Row_Transposed = nullptr;	// H_t -> IFFT(x) -> Transposed
		RWTexture2D * HeighMapTexture2D = nullptr;
		// ConstantBuffer * CB_IFFT = nullptr;
		ConstantBuffer * CB_PhillipsInit = nullptr;
		ConstantBuffer * CB_PhillipsUpdate = nullptr;
		
		// Shaders
		HlslComputeShader * CS_SpectrumInitializer = nullptr;
		HlslComputeShader * CS_SpectrumUpdater = nullptr;
		HlslComputeShader * CS_RowPassIFFT = nullptr;
		HlslComputeShader * CS_Transpose = nullptr;
		HlslComputeShader * CS_ColPassIFFT = nullptr;

		Hlsl2DTextureShader * TextureDebugShader[5] = {nullptr, };

		// HeighMap Size;
		UINT Size = 512;
	};
}
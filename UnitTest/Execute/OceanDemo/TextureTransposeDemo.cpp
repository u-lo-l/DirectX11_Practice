#include "Pch.h"
#include "TextureTransposeDemo.h"

void sdt::TextureTransposeDemo::Initialize()
{
	constexpr int ThreadX = 16;
	constexpr int ThreadY = 16;
	constexpr int ThreadZ = 1;
	ASSERT(ThreadX * ThreadY * ThreadZ < 1024, "ThreadGroup Dimension Too Large");
	const string ThreadX_str = std::to_string(ThreadX);
	const string ThreadY_str = std::to_string(ThreadY);
	const string ThreadZ_str = std::to_string(ThreadZ);
	const vector<D3D_SHADER_MACRO> ShaderMacros {
		{"THREAD_X", ThreadX_str.c_str()},
		{"THREAD_Y", ThreadY_str.c_str()},
		{"THREAD_Z", ThreadZ_str.c_str()},
		{nullptr, }
	};
	TestComputeShader = new HlslComputeShader(
		L"ComputeShader/TextureCreation.hlsl",
		ShaderMacros.data(),
		"CSMain"
	);
	
	InputTexture = new Texture(L"Original.png", true);
	ConstData.Width = InputTexture->GetWidth();
	ConstData.Height = InputTexture->GetHeight();
	// TestOutTexture = new RWTexture2D(ConstData.Width, ConstData.Height, DXGI_FORMAT_R8G8B8A8_UNORM);
	TestOutTexture = new RWTexture2D(ConstData.Width, ConstData.Height, InputTexture->GetFormat());
	CBuffer = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		&ConstData,
		"TextureDimension",
		sizeof(ConstDesc),
		true
	);

	CBuffer->BindToGPU();
	InputTexture->BindToGPU(0, static_cast<UINT>(ShaderType::ComputeShader));
	TestOutTexture->BindToGPU(0);
	TestComputeShader->Dispatch(ConstData.Width / ThreadX,ConstData.Height / ThreadY,1);
	TestOutTexture->SaveOutputAsFile(L"Transposed");
}

void sdt::TextureTransposeDemo::Destroy()
{
	// SAFE_DELETE(Original);
	// SAFE_DELETE(Transposed);
	// SAFE_DELETE(OriginalShader);
	// SAFE_DELETE(TransposedShader);

	SAFE_DELETE(InputTexture);
	SAFE_DELETE(TestOutTexture);
	SAFE_DELETE(TestComputeShader);
	SAFE_DELETE(CBuffer);
}

void sdt::TextureTransposeDemo::Tick()
{
	// if (!!OriginalShader)
	// 	OriginalShader->Tick();
	// if (!!TransposedShader)
	// 	TransposedShader->Tick();
}

void sdt::TextureTransposeDemo::Render()
{
	// if (!!TransposeComputeShader)
	// 	TransposeComputeShader->Dispatch(nullptr, 1,1,1);
	//
	// if (!!Original)
	// 	Original->BindToGPU(0, static_cast<UINT>(ShaderType::PixelShader));
	// if (!!Transposed)
	// 	Transposed->BindToGPU(1);
	// if (!!OriginalShader)
	// 	OriginalShader->Render();
	// if (!!TransposedShader)
	// 	TransposedShader->Render();
}

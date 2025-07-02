#include "framework.h"
#include "RWTexture2D.h"


RWTexture2D::RWTexture2D
(
	const UINT Width, 
	const UINT Height, 
	const DXGI_FORMAT InTextureFormat
)
: Width(Width)
, Height(Height)
, TextureFormat(InTextureFormat)
{
	ASSERT(Verify(), "Texture Format Not Valid for UAV");
	CreateOutputTexture();
	CreateUAV();
	CreateSRV();
	CreateResultTexture();
}

RWTexture2D::~RWTexture2D()
{
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(SRV);
}

// void RWTexture2D::BindToGPUAsUAV(const UINT SlotNum) const
// {
// 	if (!!UAV)
// 		D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(
// 			SlotNum,
// 			1,
// 			&UAV,
// 			nullptr
// 		);
// }

void RWTexture2D::BindToGPUAsSRV(const UINT SlotNum) const
{
	if (!SRV)
		return ;
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWTexture2D::BindToGPUAsSRV(const UINT SlotNum, const ShaderType InShaderType) const
{
	if (!SRV)
		return ;
	if(InShaderType & ShaderType::VertexShader)
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::PixelShader)
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::HullShader)
		D3D::Get()->GetDeviceContext()->HSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::DomainShader)
		D3D::Get()->GetDeviceContext()->DSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::GeometryShader)
		D3D::Get()->GetDeviceContext()->GSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::ComputeShader)
		D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWTexture2D::CreateSRV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();

	SAFE_RELEASE(SRV);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = TextureFormat;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	
	const HRESULT Hr = Device->CreateShaderResourceView(this->OutputTexture, &SRVDesc, &this->SRV);
	CHECK(SUCCEEDED(Hr));
}

UINT RWTexture2D::GetWidth() const
{ 
 	return Width;
}

UINT RWTexture2D::GetHeight() const
{ 
 	return Height;
}

ID3D11UnorderedAccessView * RWTexture2D::GetUAV() const
{
	return UAV;
}

ID3D11ShaderResourceView* RWTexture2D::GetSRV() const
{
	return SRV;
}

void RWTexture2D::SaveOutputAsFile(const wstring& FileName) const
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	OutputTexture->GetDesc(&TextureDesc);
	if (
		(TextureDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM) &&
		(TextureDesc.Format != DXGI_FORMAT_R32G32B32A32_FLOAT) &&
		(TextureDesc.Format != DXGI_FORMAT_R32G32_FLOAT) &&
		(TextureDesc.Format != DXGI_FORMAT_R32_FLOAT)
	)
		ASSERT(false, "Texture format does not valid to save as file");
		
	ID3D11DeviceContext * DeviceContext =  D3D::Get()->GetDeviceContext();
	DeviceContext->CopyResource(ResultTexture, OutputTexture);
	Helper::SaveTextureAsFile(ResultTexture, FileName);
}

void RWTexture2D::ExtractTextureColors(vector<Color>& OutPixels, const Vector2D& VertexNum) const
{
	const string Format = this->TextureFormat == DXGI_FORMAT_R32_FLOAT ? "float" : "float4"; 
	OutPixels.clear();
	OutPixels.resize(static_cast<size_t>(VertexNum.X * VertexNum.Y));
	const RawBuffer * RWBuffer = new RawBuffer(
		nullptr,
		OutPixels.size() * sizeof(Color),
		OutPixels.size() * sizeof(Color)
	);
	struct ResolutionDesc
	{
		Vector2D Resolution;
		Vector2D Padding;
	} ResolutionData = {
		VertexNum,
		Vector2D::Zero
	};
	ConstantBuffer * CB_Resolution = new ConstantBuffer(
		ShaderType::ComputeShader,
		0,
		&ResolutionData,
		"Sampling Resolution",
		sizeof(float) * 4,
		true
	);
	const int ThreadDim[2] = {
		min(8, int(VertexNum.X)),
		min(8, int(VertexNum.Y))
	};
	const string ThreadDimStr[2] = {
		to_string(ThreadDim[0]),
		to_string(ThreadDim[1])
	};
	// TODO:
	// const vector<D3D_SHADER_MACRO> Defines{
	// 		{"THREAD_X", ThreadDimStr[0].c_str()},
	// 		{"THREAD_Y", ThreadDimStr[1].c_str()},
	// 		{"FORMAT", Format.c_str()},
	// 		{nullptr, nullptr},
	// 	};
	// HlslComputeShader * TextureColorExtractor = new HlslComputeShader(L"ComputeShader/GetTextureData.hlsl", Defines.data());
	// TextureColorExtractor->CreateSamplerState_Linear_Clamp();
	ComputeShader * TextureColorExtractor = ShaderManager::Get()->GetComputeShader("TextureColorExtractor");

	CB_Resolution->BindToGPU();
	this->BindToGPUAsSRV(0, ShaderType::ComputeShader);
	RWBuffer->BindOutputToGPU(0);
	const uint32_t ThreadGroupX = static_cast<uint32_t>(ceil(VertexNum.X / static_cast<float>(ThreadDim[0])));
	const uint32_t ThreadGroupY = static_cast<uint32_t>(ceil(VertexNum.Y / static_cast<float>(ThreadDim[1])));
	// TODO:
	// TextureColorExtractor->Dispatch(TODO, TODO, TODO);
	RWBuffer->GetOutputData(OutPixels.data());

	SAFE_DELETE(RWBuffer);
	SAFE_DELETE(CB_Resolution);
}

Vector2D RWTexture2D::GetSize() const
{
	return Vector2D(static_cast<float>(Width), static_cast<float>(Height));
}

bool RWTexture2D::Verify() const
{
	D3D11_FEATURE_DATA_FORMAT_SUPPORT2 FormatSupport = {};
	FormatSupport.InFormat = this->TextureFormat;
	
	HRESULT Hr = D3D::Get()->GetDevice()->CheckFeatureSupport(
		D3D11_FEATURE_FORMAT_SUPPORT2,
		&FormatSupport,
		sizeof(FormatSupport)
	);
	return SUCCEEDED(Hr);
}

void RWTexture2D::CreateOutputTexture()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	D3D11_TEXTURE2D_DESC RWTexture2DDesc;
	ZeroMemory(&RWTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DDesc.Width = Width;
	RWTexture2DDesc.Height = Height;
	RWTexture2DDesc.MipLevels = 1;
	RWTexture2DDesc.ArraySize = 1;
	RWTexture2DDesc.Format = TextureFormat;
	RWTexture2DDesc.SampleDesc.Count = 1;
	RWTexture2DDesc.SampleDesc.Quality = 0;
	
	RWTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	RWTexture2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	RWTexture2DDesc.CPUAccessFlags = 0;
	RWTexture2DDesc.MiscFlags = 0;

	HRESULT Hr = Device->CreateTexture2D(&RWTexture2DDesc, nullptr, &OutputTexture);
	CHECK(SUCCEEDED(Hr));
}

void RWTexture2D::CreateUAV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = TextureFormat;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;
	HRESULT Hr = Device->CreateUnorderedAccessView(OutputTexture, &UAVDesc,	&UAV);
	CHECK(SUCCEEDED(Hr));
}

void RWTexture2D::CreateResultTexture()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	
	D3D11_TEXTURE2D_DESC RWTexture2DDesc;
	ZeroMemory(&RWTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DDesc.Width = Width;
	RWTexture2DDesc.Height = Height;
	RWTexture2DDesc.MipLevels = 1;
	RWTexture2DDesc.ArraySize = 1;
	RWTexture2DDesc.Format = TextureFormat;
	RWTexture2DDesc.SampleDesc.Count = 1;
	RWTexture2DDesc.SampleDesc.Quality = 0;
	
	RWTexture2DDesc.Usage = D3D11_USAGE_STAGING;
	RWTexture2DDesc.BindFlags = 0;
	RWTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	RWTexture2DDesc.MiscFlags = 0;

	const HRESULT Hr = Device->CreateTexture2D(&RWTexture2DDesc, nullptr, &ResultTexture);
	CHECK(SUCCEEDED(Hr));
}

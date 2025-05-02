#include "framework.h"
#include "Texture.h"


void Texture::SaveTextureAsFile(ID3D11Texture2D * InTexture, const wstring& FileName)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DirectX::ScratchImage Image;
	// 내부적으로 InTexture를 Staging으로 변환한다.
	HRESULT Hr = DirectX::CaptureTexture(Device, DeviceContext, InTexture, Image);
	CHECK(SUCCEEDED(Hr));
	
	Hr = DirectX::SaveToWICFile(
		*(Image.GetImage(0,0,0)),
		DirectX::WIC_FLAGS_NONE,
		DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
		(wstring(W_TEXTURE_PATH) + FileName + L".png").c_str()
	);
	CHECK(SUCCEEDED(Hr));
}

Texture::Texture( const wstring & FileName, bool bDefaultPath )
	: SRV(nullptr), TexMeta(), FileName(FileName)
{
	wstring FullPath = bDefaultPath == true ? W_TEXTURE_PATH + FileName : FileName;  
	this->FileName = Path::GetFileName( FullPath );
	
	CHECK(LoadTextureAndCreateSRV(FullPath) >= 0);
}

Texture::Texture(ID3D11Texture2D* InTexture, const D3D11_TEXTURE2D_DESC& Desc)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = Desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	HRESULT Hr = Device->CreateShaderResourceView(
		InTexture,
		&srvDesc,
		&this->SRV
	);

	TexMeta.width = Desc.Width;
	TexMeta.height = Desc.Height;
	TexMeta.depth = 1;
	TexMeta.mipLevels = 1;
	TexMeta.arraySize = 1;
	TexMeta.format = Desc.Format;

	CHECK(SUCCEEDED(Hr));
}

Texture::~Texture()
{
	SAFE_RELEASE(this->SRV);
}

void Texture::BindToGPU(UINT SlotNum, UINT InShaderType) const
{
	if(InShaderType & static_cast<UINT>(ShaderType::VertexShader))
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(SlotNum, 1, &this->SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::PixelShader))
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(SlotNum, 1, &this->SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::HullShader))
		D3D::Get()->GetDeviceContext()->HSSetShaderResources(SlotNum, 1, &this->SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::DomainShader))
		D3D::Get()->GetDeviceContext()->DSSetShaderResources(SlotNum, 1, &this->SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::ComputeShader))
		D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &this->SRV);
}

HRESULT Texture::LoadTextureAndCreateSRV(const wstring & FullPath)
{
	DirectX::ScratchImage ScratchImage;
	const wstring Extension = Path::GetExtension(FullPath);
	if (Extension == L"tga")
	{
		CHECK(LoadFromTGAFile(FullPath.c_str(), &TexMeta, ScratchImage) >= 0);
	}
	else if (Extension == L"dds")
	{
		CHECK(LoadFromDDSFile(FullPath.c_str(), DirectX::DDS_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}
	else
	{
		CHECK(LoadFromWICFile(FullPath.c_str(), DirectX::WIC_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}
	// SRV 생성
	return CreateShaderResourceView(
		D3D::Get()->GetDevice(),
		ScratchImage.GetImages(),
		ScratchImage.GetImageCount(),
		TexMeta,
		&this->SRV
	);
}

void Texture::ExtractTextureColors( vector<Color> & OutPixels ) const
{
	OutPixels.clear();

	ID3D11Texture2D * SourceTexture = nullptr;
	SRV->GetResource(reinterpret_cast<ID3D11Resource **>(&SourceTexture));

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	SourceTexture->GetDesc(&TextureDesc);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Usage = D3D11_USAGE_STAGING; // CPU, GPU 둘 다 읽고 쓸 수 있음. (가장 느림)
	TextureDesc.BindFlags = 0; // STAGING에선 BindFlag 0이어야 함.
	TextureDesc.MiscFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	
	ID3D11Texture2D * DestTexture;
	CHECK(D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, &DestTexture) >= 0);
	CHECK(LoadTextureFromTexture(SourceTexture, &DestTexture) >= 0);
	
	const UINT PixelCount = TextureDesc.Width * TextureDesc.Height;
	UINT * Colors = new UINT[PixelCount];
	D3D11_MAPPED_SUBRESOURCE SubResource;
	CHECK(D3D::Get()->GetDeviceContext()->Map(DestTexture, 0, D3D11_MAP_READ, 0, &SubResource) >= 0);
	memcpy(Colors, SubResource.pData, PixelCount * sizeof(UINT));
	D3D::Get()->GetDeviceContext()->Unmap(DestTexture, 0);

	OutPixels.reserve(PixelCount);
	for (UINT i = 0; i < PixelCount; i++)
		OutPixels.emplace_back(Colors[i]);

	SAFE_DELETE_ARR(Colors);
	SAFE_RELEASE(DestTexture);
}

// D3DX11LoadTextureFromTexture : D3D11X.h . 더이상 권장되지 않음.
// CHECK(D3DX11LoadTextureFromTexture(D3D::Get()->GetDeviceContext(), InSourceTexture, nullptr, Texture) >= 0);
HRESULT Texture::LoadTextureFromTexture(
	ID3D11Texture2D * InSourceTexture,
	ID3D11Texture2D ** OutTargetTexture
)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const Context = D3D::Get()->GetDeviceContext();
	*OutTargetTexture = nullptr;
	
	D3D11_RESOURCE_DIMENSION resourceType;
	InSourceTexture->GetType(&resourceType);

	if (resourceType != D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
		return E_INVALIDARG; // 텍스처가 2D 텍스처가 아니라면 오류
	}

	// 텍스처의 크기와 포맷을 가져오기 위해 원본 텍스처의 디스크립터 생성
	ID3D11Texture2D* srcTexture2D = nullptr;
	HRESULT Hr = InSourceTexture->QueryInterface(&srcTexture2D);
	if (FAILED(Hr)) {
		return Hr;
	}

	// 새로운 텍스처 생성 (원본과 동일한 크기, 포맷, MIP 레벨 등을 사용)
	D3D11_TEXTURE2D_DESC StagingTextDesc;
	srcTexture2D->GetDesc(&StagingTextDesc);
	StagingTextDesc.Usage = D3D11_USAGE_STAGING;
	StagingTextDesc.BindFlags = 0;
	StagingTextDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	StagingTextDesc.MiscFlags = 0;

	Hr = Device->CreateTexture2D(&StagingTextDesc, nullptr, OutTargetTexture);
	if (FAILED(Hr)) {
		return Hr;
	}

	// 텍스처 데이터를 복사
	Context->CopyResource(*OutTargetTexture, InSourceTexture);

	// 최종적으로 텍스처를 반환

	return S_OK;

}

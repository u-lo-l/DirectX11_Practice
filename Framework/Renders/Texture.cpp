#include "framework.h"
#include "Texture.h"

Texture::Texture( const wstring & FileName, const DirectX::TexMetadata * InLoadInfo, bool bDefaultPath )
	: SRV(nullptr), TexMeta(), FileName(FileName)
{
	wstring FullPath = FileName;
	if ( bDefaultPath == true)
		FullPath = W_TEXTURE_PATH + FileName;
	
	LoadMetadata(InLoadInfo);
	LoadTexture();

	FullPath = Path::GetFileName( FullPath );
}

Texture::~Texture()
{
	SAFE_RELEASE(this->SRV);
}

void Texture::LoadMetadata( const DirectX::TexMetadata * InLoadInfo)
{
	const wstring Extension = Path::GetExtension(this->FileName);
	if (Extension == L"tga")
	{
		CHECK(GetMetadataFromTGAFile(FileName.c_str(), TexMeta) >= 0);
	}
	else if (Extension == L"dds")
	{
		CHECK(GetMetadataFromDDSFile(FileName.c_str(), DirectX::DDS_FLAGS_NONE, TexMeta) >= 0);
	}
	else if (Extension == L"hdr")
	{
		assert(false);
	}
	else
	{
		CHECK(GetMetadataFromWICFile(FileName.c_str(), DirectX::WIC_FLAGS_NONE, TexMeta) >= 0);
	}

	if (InLoadInfo != nullptr)
	{
		TexMeta.width = InLoadInfo->width;
		TexMeta.height = InLoadInfo->height;
	}
}

void Texture::LoadTexture()
{
	DirectX::ScratchImage ScratchImage;
	const wstring Extension = Path::GetExtension(this->FileName);
	if (Extension == L"tga")
	{
		CHECK(LoadFromTGAFile(FileName.c_str(), &TexMeta, ScratchImage) >= 0);
	}
	else if (Extension == L"dds")
	{
		CHECK(LoadFromDDSFile(FileName.c_str(), DirectX::DDS_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}
	else if (Extension == L"hdr")
	{
		assert(false);
	}
	else
	{
		CHECK(LoadFromWICFile(FileName.c_str(), DirectX::WIC_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}

	// SRV
	CHECK(CreateShaderResourceView(
		D3D::Get()->GetDevice(),
		ScratchImage.GetImages(),
		ScratchImage.GetImageCount(),
		TexMeta,
		&this->SRV) >= 0);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(vector<Color>& OutPixels)
{
	return this->ReadPixels(DXGI_FORMAT_UNKNOWN, OutPixels);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels( DXGI_FORMAT InFormat, vector<Color> & OutPixels )
{
	ID3D11Texture2D * Texture2D = nullptr;
	SRV->GetResource(reinterpret_cast<ID3D11Resource **>(&Texture2D));
	if (InFormat == DXGI_FORMAT_UNKNOWN)
	{
		D3D11_TEXTURE2D_DESC Desc;
		Texture2D->GetDesc(&Desc);
		InFormat = Desc.Format;
	}
	return ReadPixels(Texture2D, InFormat, OutPixels);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels( ID3D11Texture2D * InSourceTexture, const DXGI_FORMAT InFormat, vector<Color> & OutPixels )
{
	D3D11_TEXTURE2D_DESC SourceDesc = {};
	InSourceTexture->GetDesc(&SourceDesc);

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));
	TextureDesc.Width = SourceDesc.Width;
	TextureDesc.Height = SourceDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = InFormat;
	TextureDesc.SampleDesc = SourceDesc.SampleDesc;
	TextureDesc.Usage = D3D11_USAGE_STAGING; // CPU, GPU 둘 다 읽고 쓸 수 있음. (가장 느림)
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D * Texture;
	CHECK(D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, &Texture) >= 0);
	// D3DX11LoadTextureFromTexture : D3D11X.h . 더이상 권장되지 않음.
	// CHECK(D3DX11LoadTextureFromTexture(D3D::Get()->GetDeviceContext(), InSourceTexture, nullptr, Texture) >= 0);
	CHECK(LoadTextureFromTexture(InSourceTexture, &Texture, nullptr) >= 0);
	
	UINT * Colors = new UINT[TextureDesc.Width * TextureDesc.Height];
	D3D11_MAPPED_SUBRESOURCE SubResource;
	CHECK(D3D::Get()->GetDeviceContext()->Map(Texture, 0, D3D11_MAP_READ, 0, &SubResource) >= 0);
	memcpy(Colors, SubResource.pData, TextureDesc.Width * TextureDesc.Height * sizeof(UINT));
	D3D::Get()->GetDeviceContext()->Unmap(Texture, 0);

	for (UINT Y = 0 ; Y < TextureDesc.Height ; Y++)
	{
		for (UINT X = 0 ; X < TextureDesc.Width ; X++)
		{
			const UINT Index = Y * TextureDesc.Width + X;
			OutPixels.push_back(Color(Colors[Index]));
		}
	}

	SAFE_DELETE_ARR(Colors);
	SAFE_RELEASE(Texture);
	
	return TextureDesc;
}

HRESULT Texture::LoadTextureFromTexture(
	ID3D11Texture2D * InSourceTexture,
	ID3D11Texture2D ** OutTargetTexture,
	ID3D11ShaderResourceView ** OutTargetSRV )
{
	HRESULT Hr = S_OK;
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const Context = D3D::Get()->GetDeviceContext();

	
	D3D11_RESOURCE_DIMENSION resourceType;
	InSourceTexture->GetType(&resourceType);

	if (resourceType != D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
		return E_INVALIDARG; // 텍스처가 2D 텍스처가 아니라면 오류
	}

	// 텍스처의 크기와 포맷을 가져오기 위해 원본 텍스처의 디스크립터 생성
	ID3D11Texture2D* srcTexture2D = nullptr;
	Hr = InSourceTexture->QueryInterface(&srcTexture2D);
	if (FAILED(Hr)) {
		return Hr;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	srcTexture2D->GetDesc(&texDesc);

	// 새로운 텍스처 생성 (원본과 동일한 크기, 포맷, MIP 레벨 등을 사용)
	D3D11_TEXTURE2D_DESC newTexDesc = texDesc;
	newTexDesc.Usage = D3D11_USAGE_DEFAULT;
	newTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	newTexDesc.CPUAccessFlags = 0;

	ID3D11Texture2D* newTexture = nullptr;
	Hr = Device->CreateTexture2D(&newTexDesc, nullptr, &newTexture);
	if (FAILED(Hr)) {
		return Hr;
	}

	// 텍스처 데이터를 복사
	Context->CopyResource(newTexture, InSourceTexture);

	// 텍스처에 대한 쉐이더 리소스 뷰 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	Hr = Device->CreateShaderResourceView(newTexture, &srvDesc, nullptr);
	if (FAILED(Hr)) {
		newTexture->Release();
		return Hr;
	}

	// 최종적으로 텍스처를 반환
	*OutTargetTexture = newTexture;

	return S_OK;

}

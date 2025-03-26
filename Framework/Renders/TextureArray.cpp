#include "framework.h"
#include "TextureArray.h"

TextureArray::TextureArray
(
	const vector<wstring> & InNames,
	UINT InWidth,
	UINT InHeight,
	UINT InMipLevels
)
{
	vector<wstring> TempTextureNames;
	TempTextureNames.reserve(InNames.size());

	for (const wstring & Name : InNames)
		TempTextureNames.emplace_back(W_TEXTURE_PATH + Name);

	vector<ID3D11Texture2D*> TempTextures = CreateTexture(TempTextureNames, InWidth, InHeight, InMipLevels);
	
	D3D11_TEXTURE2D_DESC TextureDesc;
	TempTextures[0]->GetDesc(&TextureDesc); // 왜 0번째 Texture만?
	const UINT ArraySize = TempTextureNames.size();
	TextureDesc.ArraySize = ArraySize;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;
	
	ID3D11Texture2D* Texture2DArray;
	CHECK(D3D::Get()->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, &Texture2DArray) >= 0);

	for (UINT i = 0; i < TempTextures.size(); i++)
	{
		for (UINT level = 0; level < TextureDesc.MipLevels; level++)
		{
			D3D11_MAPPED_SUBRESOURCE subResource;
			if (D3D::Get()->GetDeviceContext()->Map(TempTextures[i], level, D3D11_MAP_READ, 0, &subResource) >= 0)
			{
				UINT Dest = D3D11CalcSubresource(level, i, TextureDesc.MipLevels);
				D3D::Get()->GetDeviceContext()->UpdateSubresource(Texture2DArray, Dest, nullptr, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
				D3D::Get()->GetDeviceContext()->Unmap(TempTextures[i], level);
			}
		}
	}

	// Texture2DArray에 대한 SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = TextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = TextureDesc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = ArraySize;
	CHECK(D3D::Get()->GetDevice()->CreateShaderResourceView(Texture2DArray, &SRVDesc, &SRV) >= 0);

	// 지역변수 정리
	for (ID3D11Texture2D* Texture : TempTextures)
		SAFE_RELEASE(Texture);
	SAFE_RELEASE(Texture2DArray);
}

TextureArray::~TextureArray()
{
	SAFE_RELEASE(this->SRV);
}

void TextureArray::BindToGPU(UINT SlotNum) const
{
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(SlotNum, 1, &this->SRV);
}

// TODO : 깔끔하게 정리좀 하자.
vector<ID3D11Texture2D*> TextureArray::CreateTexture
(
	const vector<wstring>& InNames,
	UINT InWidth,
	UINT InHeight,
	UINT InMipLevels
)
{
	vector<ID3D11Texture2D*> returnTextures;
	returnTextures.resize(InNames.size());

	for (UINT i = 0; i < returnTextures.size(); i++)
	{
		HRESULT hr;

		DirectX::TexMetadata metaData;
		wstring ext = Path::GetExtension(InNames[i]);
		if (ext == L"tga")
		{
			hr = GetMetadataFromTGAFile(InNames[i].c_str(), metaData);
			CHECK(hr >= 0);
		}
		else if (ext == L"dds")
		{
			hr = GetMetadataFromDDSFile(InNames[i].c_str(), DirectX::DDS_FLAGS_NONE, metaData);
			CHECK(hr >= 0);
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = GetMetadataFromWICFile(InNames[i].c_str(), DirectX::WIC_FLAGS_NONE, metaData);
			CHECK(hr >= 0);
		}


		DirectX::ScratchImage image;
		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(InNames[i].c_str(), &metaData, image);
			CHECK(hr >= 0);
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(InNames[i].c_str(), DirectX::DDS_FLAGS_NONE, &metaData, image);
			CHECK(hr >= 0);
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(InNames[i].c_str(), DirectX::WIC_FLAGS_NONE, &metaData, image);
			CHECK(hr >= 0);
		}


		DirectX::ScratchImage resizedImage;
		hr = Resize(image.GetImages(), image.GetImageCount(), image.GetMetadata(), InWidth, InHeight, DirectX::TEX_FILTER_DEFAULT, resizedImage);
		CHECK(hr >= 0);


		if (InMipLevels > 1)
		{
			DirectX::ScratchImage mipmapedImage;
			hr = DirectX::GenerateMipMaps(resizedImage.GetImages(), resizedImage.GetImageCount(), resizedImage.GetMetadata(), DirectX::TEX_FILTER_POINT, InMipLevels, mipmapedImage);
			CHECK(hr >= 0);

			hr = DirectX::CreateTextureEx
			(
				D3D::Get()->GetDevice()
				, mipmapedImage.GetImages()
				, mipmapedImage.GetImageCount()
				, mipmapedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource**)&returnTextures[i]
			);
			CHECK(hr >= 0);
		}
		else
		{
			hr = DirectX::CreateTextureEx
			(
				D3D::Get()->GetDevice()
				, resizedImage.GetImages()
				, resizedImage.GetImageCount()
				, resizedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource**)&returnTextures[i]
			);
			CHECK(hr >= 0);
		}
	}//for(i)

	return returnTextures;
}

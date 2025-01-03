#include "framework.h"
#include "Texture.h"

Texture::Texture( const wstring & FileName, D3DX11_IMAGE_INFO * InLoadInfo )
	: SRV(nullptr), TexMeta(), FileName(FileName)
{
	if ( Path::IsRelativePath(FileName) == true)
		this->FileName = L"../../_Textures/" + FileName;
	LoadMetadata(InLoadInfo);
	LoadTexture();
	
	String::Replace(&this->FileName,  L"../../_Textures/", L"");
}

Texture::~Texture()
{
	SAFE_RELEASE(this->SRV);
}

void Texture::LoadMetadata(D3DX11_IMAGE_INFO * InLoadInfo)
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

	UINT Width = TexMeta.width;
	UINT Height = TexMeta.height;
	if (InLoadInfo != nullptr)
	{
		Width = InLoadInfo->Width;
		Height = InLoadInfo->Height;
		TexMeta.width = InLoadInfo->Width;
		TexMeta.height = InLoadInfo->Height;
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
	CHECK(D3DX11LoadTextureFromTexture(D3D::Get()->GetDeviceContext(), InSourceTexture, nullptr, Texture) >= 0);
	
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

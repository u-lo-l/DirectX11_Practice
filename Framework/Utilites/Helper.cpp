#include "framework.h"
#include "Helper.h"

HRESULT Helper::CreateShaderResourceViewFromFile
(
	ID3D11Device* Device,
	const wstring& SrcFileName,
	ID3D11ShaderResourceView** OutShaderResourceView
)
{
	DirectX::ScratchImage ScratchImage;
	DirectX::TexMetadata TexMeta;
	
	const wstring Extension = Path::GetExtension(SrcFileName);
	if (Extension == L"tga")
	{
		CHECK(LoadFromTGAFile(SrcFileName.c_str(), &TexMeta, ScratchImage) >= 0);
	}
	else if (Extension == L"dds")
	{
		CHECK(LoadFromDDSFile(SrcFileName.c_str(), DirectX::DDS_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}
	else
	{
		CHECK(LoadFromWICFile(SrcFileName.c_str(), DirectX::WIC_FLAGS_NONE, &TexMeta, ScratchImage) >= 0);
	}
	// SRV 생성
	return CreateShaderResourceView(
		Device,
		ScratchImage.GetImages(),
		ScratchImage.GetImageCount(),
		TexMeta,
		OutShaderResourceView
	);
}

string Helper::ColorToJson( const Color & InColor )
{
	return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
}

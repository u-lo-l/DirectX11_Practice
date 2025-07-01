#include "framework.h"
#include "Helper.h"

string Helper::ColorToJson( const Color & InColor )
{
	return String::Format("%0.6f,%0.6f,%0.6f,%0.6f", InColor.R, InColor.G, InColor.B, InColor.A);
}

Color Helper::JsonToColor(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 4);

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

Vector Helper::JsonToVector3(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 3);
	
	return Vector(stof(v[0]), stof(v[1]), stof(v[2]));
}

Quaternion Helper::JsonToQuaternion(const string & InJsonString)
{
	vector<Json::String> v;
	String::SplitString(&v, InJsonString, ",");

	CHECK(v.size() == 4);
	
	return Quaternion(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

Vector Helper::ConvertRhZUpNormalToLhYUp(const Vector& InNormal)
{
	Vector Normal;
	Normal.X = InNormal.X;
	Normal.Y = InNormal.Z;
	Normal.Z = -InNormal.Y;
	return Normal;
}

void Helper::SaveTextureAsFile(ID3D11Texture2D * InTexture, const wstring& FileName, UINT Slice)
{
	CHECK(!!InTexture);
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DirectX::ScratchImage Image;
	// 내부적으로 InTexture를 Staging으로 변환한다.
	HRESULT Hr = DirectX::CaptureTexture(Device, DeviceContext, InTexture, Image);
	CHECK(SUCCEEDED(Hr));

	DXGI_FORMAT originalFormat = Image.GetMetadata().format;

	if (
		(originalFormat != DXGI_FORMAT_R8G8B8A8_UNORM) &&
		(originalFormat != DXGI_FORMAT_R32G32B32A32_FLOAT) &&
		(originalFormat != DXGI_FORMAT_R32G32_FLOAT) &&
		(originalFormat != DXGI_FORMAT_R32_FLOAT)
	)
	{
		ASSERT(false, "Texture format does not valid to save as file");
	}


	wstring FilePath = wstring(W_TEXTURE_PATH) + FileName + L".png";
	wstring Dir = Path::GetDirectoryName(FilePath);
	if (Path::IsDirectoryExist(Dir) == false)
		Path::CreateFolder(Dir);
	if (originalFormat == DXGI_FORMAT_R32G32_FLOAT)
	{
		const DirectX::Image * img = Image.GetImage(0, Slice, 0);
		DirectX::ScratchImage newImage;
		Hr = newImage.Initialize2D(
			DXGI_FORMAT_R32G32B32A32_FLOAT, // 목표 포맷
			img->width,
			img->height,
			1,
			0
		);
		CHECK(SUCCEEDED(Hr));

		// 픽셀 데이터 변환
		img->pixels;
		Vector2D* originalPixels = reinterpret_cast<Vector2D*>(img->pixels);
		Vector4* newPixels = reinterpret_cast<Vector4*>(newImage.GetImage(0, 0, 0)->pixels);

		size_t pixelCount = img->width * img->height;
		for (size_t i = 0; i < pixelCount; ++i)
		{
			newPixels[i].X = originalPixels[i].X;
			newPixels[i].Y = originalPixels[i].Y;
			newPixels[i].Z = 0.0f;
			newPixels[i].W = 1.0f;
		}
		Hr = DirectX::SaveToWICFile(
			*(newImage.GetImage(0,0,0)),
			DirectX::WIC_FLAGS_NONE,
			DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
			FilePath.c_str()
		);
	}
	else
	{
		Hr = DirectX::SaveToWICFile(
			*(Image.GetImage(0,Slice, 0)),
			DirectX::WIC_FLAGS_NONE,
			DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
			FilePath.c_str()
		);
	}
	CHECK(SUCCEEDED(Hr));
}

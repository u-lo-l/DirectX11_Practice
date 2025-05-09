#pragma once

class Helper
{
public:
	// static HRESULT CreateShaderResourceViewFromFile
	// (
	// 	ID3D11Device * Device,
	// 	const wstring & SrcFileName,
	// 	ID3D11ShaderResourceView** OutShaderResourceView
	// );

	static string ColorToJson(const class Color & InColor);
};

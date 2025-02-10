#include "framework.h"
#include "HlslShader.h"

HlslShader::HlslShader(const wstring & VsFileName, const wstring & PsFileName, const wstring & CsFileName)
: DeviceContext(D3D::Get()->GetDeviceContext())
, VertexShader(nullptr)
, PixelShader(nullptr)
, ComputeShader(nullptr)
, InputLayout(nullptr)
{
	if (VsFileName.length())
		CompileShader(ShaderType::Vertex, VsFileName);
	if (VsFileName.length())
		CompileShader(ShaderType::Pixel, PsFileName);
	if (VsFileName.length())
		CompileShader(ShaderType::Compute, CsFileName);

	// VertexShader의 VertexInput구조체랑 같아야 한다.
	// ModelVertex를 쓸거면 요소가 7개라 7개 만들어줘야 함. 홀리
	// Effect에선 for문 돌 수 있었는데 ㄲㅂ
	D3D11_INPUT_ELEMENT_DESC InputLayoutDescs[7];
	// InputLayoutDescs[0].SemanticName = "POSITION";
	// InputLayoutDescs[0].SemanticIndex = 0;
	// InputLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	// InputLayoutDescs[0].InputSlot = VertexSlot;
	// InputLayoutDescs[0].AlignedByteOffset = 0;
	// InputLayoutDescs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	// InputLayoutDescs[0].InstanceDataStepRate = 0;
}

HlslShader::~HlslShader()
{
	SAFE_RELEASE(VertexShader);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(ComputeShader);
	SAFE_RELEASE(InputLayout);
}

void HlslShader::CompileShader( ShaderType Type, const wstring & ShaderFileName )
{
	ID3DBlob * ErrorBlob = nullptr;
	ID3DBlob * ShaderBlob = nullptr;
	
	INT flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	flag |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	
	HRESULT hr = D3DCompileFromFile(
		ShaderFileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		GetEntryPoint(Type).c_str(),
		GetShaderTarget(Type).c_str(),
		flag, // https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/d3dcompile-constants
		0, // Effect파일을 컴파일하는 방법.
		&ShaderBlob,
		&ErrorBlob
	);
	if (FAILED(hr) && ErrorBlob != nullptr)
	{
		ASSERT(false, VsFileName + ::GetD3D11ReturnMessage(hr));
		return ;
	}
	if (FAILED(hr) && ErrorBlob == nullptr)
	{
		ASSERT(false, VsFileName + L" Failed to Compile");
		return ;
	}
	
}

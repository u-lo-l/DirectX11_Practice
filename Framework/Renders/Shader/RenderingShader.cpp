#include "framework.h"
#include <fstream>
#include "RenderingShader.h"


RenderingShader::RenderingShader( const RenderingShaderDesc& InDesc )
	: Desc(InDesc)
{
	// string OutMessage;
	// ASSERT(Verify(InDesc,OutMessage) == true, OutMessage.c_str());
	
	ShaderManager * const ShaderManagerInst = ShaderManager::Get();
	ASSERT(!Desc.ShaderFileName.empty(), "Shader Name Empty");
	
	Desc.ShaderFileName = W_SHADER_PATH + Desc.ShaderFileName;
	const wstring ShaderDirectory = Path::GetDirectoryName(Desc.ShaderFileName);
	Desc.PreCompiledShaderFileDirectory = ShaderDirectory + L"PreCompiled/";

	Pass.Topology = Desc.Topology;
	
	if (Desc.TargetShaderType & ShaderType::PixelShader)
		LoadShader(ShaderType::PixelShader);
	if (Desc.TargetShaderType & ShaderType::VertexShader)
		LoadShader(ShaderType::VertexShader);
	if (Desc.TargetShaderType & ShaderType::DomainShader)
		LoadShader(ShaderType::DomainShader);
	if (Desc.TargetShaderType & ShaderType::HullShader)
		LoadShader(ShaderType::HullShader);
	if (Desc.TargetShaderType & ShaderType::GeometryShader)
		LoadShader(ShaderType::GeometryShader);
	
	for ( const auto & Item : Desc.SamplerStateNames)
	{
		int RegisterIndex = std::get<0>(Item);
		ShaderType TargetShader = std::get<1>(Item);
		const string & SamplerStateName = std::get<2>(Item);
		ID3D11SamplerState * SamplerState = ShaderManagerInst->GetSamplerState(SamplerStateName);
		ASSERT(!!SamplerState, String::Format("Failed to find SamplerState : %s", SamplerStateName).c_str());
		Pass.SamplerStates[RegisterIndex] = {SamplerState, TargetShader };
	}
	Pass.RasterizerState = ShaderManagerInst->GetRasterizerState(Desc.RasterizerStateName);
	ASSERT(!!Pass.RasterizerState, String::Format("Failed to find Rasterizer : %s", Desc.RasterizerStateName).c_str());
	Pass.BlendState = ShaderManagerInst->GetBlendState(Desc.BlendStateName);
	ASSERT(!!Pass.BlendState, String::Format("Failed to find BlendState : %s", Desc.BlendStateName).c_str());
	Pass.DepthStencilState = ShaderManagerInst->GetDepthStencilState(Desc.DepthStencilStateName);
	ASSERT(!!Pass.DepthStencilState, String::Format("Failed to find DepthStencilState : %s", Desc.DepthStencilStateName).c_str());
}

RenderingShader::~RenderingShader()
{
	SAFE_RELEASE(Pass.VertexShader)
	SAFE_RELEASE(Pass.PixelShader)
	SAFE_RELEASE(Pass.GeometryShader)
	SAFE_RELEASE(Pass.HullShader)
	SAFE_RELEASE(Pass.DomainShader)
}

ID3D11InputLayout * RenderingShader::GetInputLayout()
{
	return Pass.InputLayout;
}

const ID3D11InputLayout * RenderingShader::GetInputLayout() const
{
	return Pass.InputLayout;
}

// const ID3D11InputLayout* RenderingShader::GetInputLayout() const
// {
// 	return Pass.InputLayout;
// }

D3D_PRIMITIVE_TOPOLOGY RenderingShader::GetTopology() const
{
	return Pass.Topology;
}

void RenderingShader::Draw(VertexBuffer* VB, IndexBuffer* IB, InstanceBuffer* InstB)
{
	const UINT VertexCount = !!VB ? VB->GetCount() : 0;
	const UINT IndexCount = !!IB ? IB->GetCount() : 0;
	const UINT InstanceCount = !!InstB ? InstB->GetCount() : 0;

	if (VertexCount == 0)
		return;
	
	if (!!VB) VB->BindToGPU();
	if (!!IB) IB->BindToGPU();
	if (!!InstB) InstB->BindToGPU();
	
	if (InstB != nullptr && IB != nullptr)
		DrawIndexedInstanced(IndexCount, InstanceCount);
	else if (InstB != nullptr && IB == nullptr)
		DrawInstanced(VertexCount, InstanceCount);
	else if (InstB == nullptr && IB != nullptr)
		DrawIndexed(IndexCount);
	else
		Draw(VertexCount);
}

void RenderingShader::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	D3D::Get()->GetDeviceContext()->Draw(VertexCount, StartVertexLocation);
}

void RenderingShader::DrawIndexed(const UINT IndexCount, const UINT StartIndexLocation, const int BaseVertexLocation)
{
	D3D::Get()->GetDeviceContext()->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void RenderingShader::DrawInstanced
(
	const UINT VertexCountPerInstance,
	const UINT InstanceCount,
	const UINT StartVertexLocation,
	const UINT StartInstanceLocation
)
{
	D3D::Get()->GetDeviceContext()->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void RenderingShader::DrawIndexedInstanced
(
	const UINT IndexCountPreInstance,
	const UINT InstanceCount,
	const UINT StartIndexLocation,
	const int BaseVertexLocation,
	const UINT StartInstanceLocation
)
{
	D3D::Get()->GetDeviceContext()->DrawIndexedInstanced(
		IndexCountPreInstance,
		InstanceCount,
		StartIndexLocation,
		BaseVertexLocation,
		StartInstanceLocation
	);
}

void RenderingShader::InitializeInputLayout(ID3DBlob * InVertexShaderBlob)
{
	const size_t NumElements = Desc.pInputLayoutElements->size();
	ASSERT(NumElements > 0, "Input Layout Elements Not Valid");

	const HRESULT Hr = D3D::Get()->GetDevice()->CreateInputLayout(
		Desc.pInputLayoutElements->data(),
		NumElements,
		InVertexShaderBlob->GetBufferPointer(),
		InVertexShaderBlob->GetBufferSize(),
		&Pass.InputLayout
	);
	if (FAILED(Hr))
	{
		char* errorMessage = nullptr;

		// HRESULT로부터 에러 메시지를 얻기
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			Hr,
			0,
			(LPSTR)&errorMessage,
			0,
			NULL
		);
		std::string errorStr = errorMessage;
		// 메모리 해제
		LocalFree(errorMessage);
	}
	ASSERT((Hr >= 0), "Failed to create input layout")
}

bool RenderingShader::Verify(const RenderingShaderDesc& InDesc, string & OutMessage)
{
	OutMessage = "";
	if (InDesc.ShaderFileName.empty())
		OutMessage = "Shader file not specified";
	else if (InDesc.pInputLayoutElements->size() == 0)
		OutMessage = "Input Layout is empty";
	else if (
		D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST <= InDesc.Topology &&
		InDesc.Topology <= D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST &&
		(InDesc.TargetShaderType & ShaderType::HD) == false 
	)
		OutMessage = "Tessellation Stage Needed";
	
	return OutMessage == "";
}

// TODO : MultiPass
void RenderingShader::SetPass(int PassIndex) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DeviceContext->IASetPrimitiveTopology(Pass.Topology);
	ASSERT(!!Pass.InputLayout, "Failed to set input layout");
	DeviceContext->IASetInputLayout(Pass.InputLayout);

	if (!!Pass.VertexShader)
		DeviceContext->VSSetShader(Pass.VertexShader, nullptr, 0);
	if (!!Pass.HullShader)
		DeviceContext->HSSetShader(Pass.HullShader, nullptr, 0);
	if (!!Pass.DomainShader)
		DeviceContext->DSSetShader(Pass.DomainShader, nullptr, 0);
	if (!!Pass.GeometryShader)
		DeviceContext->GSSetShader(Pass.GeometryShader, nullptr, 0);
	if (!!Pass.PixelShader)
	{
		DeviceContext->PSSetShader(Pass.PixelShader, nullptr, 0);
		if (!!Pass.BlendState)
		{
			float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			DeviceContext->OMSetBlendState(Pass.BlendState, BlendFactor, 0xFFFFFFFF);
		}
		if (!!Pass.DepthStencilState)
		{
			DeviceContext->OMSetDepthStencilState(Pass.DepthStencilState, 0);
		}
	}
	for ( const auto & Pair : Pass.SamplerStates)
	{
		const int & RegisterIndex = Pair.first;
		const ShaderType & Type = Pair.second.TargetShader;
		ID3D11SamplerState * Sampler = Pair.second.SamplerState;
		if (Type & ShaderType::VertexShader)
			DeviceContext->VSSetSamplers(RegisterIndex, 1, &Sampler);
		if (Type & ShaderType::GeometryShader)
			DeviceContext->GSSetSamplers(RegisterIndex, 1, &Sampler);
		if (Type & ShaderType::DomainShader)
			DeviceContext->DSSetSamplers(RegisterIndex, 1, &Sampler);
		if (Type & ShaderType::HullShader)
			DeviceContext->HSSetSamplers(RegisterIndex, 1, &Sampler);
		if (Type & ShaderType::PixelShader)
			DeviceContext->PSSetSamplers(RegisterIndex, 1, &Sampler);
	}
	if (!!Pass.RasterizerState)
	{
		DeviceContext->RSSetState(Pass.RasterizerState);
	}
}

void RenderingShader::ClearPass()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	
	DeviceContext->IASetInputLayout(nullptr);
	DeviceContext->VSSetShader(nullptr, nullptr, 0);
	DeviceContext->HSSetShader(nullptr, nullptr, 0);
	DeviceContext->DSSetShader(nullptr, nullptr, 0);
	DeviceContext->GSSetShader(nullptr, nullptr, 0);
	DeviceContext->PSSetShader(nullptr,  nullptr, 0);
	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DeviceContext->OMSetBlendState(nullptr, BlendFactor, 0xFFFFFFFF);
	DeviceContext->OMSetDepthStencilState(nullptr, 0);
}

wstring RenderingShader::GetEntryPoint(const ShaderType Type) const
{
	switch (Type)
	{
	case ShaderType::VertexShader:	 return String::ToWString(Desc.EntryPoints.VSEntryPoint);
	case ShaderType::PixelShader:	 return String::ToWString(Desc.EntryPoints.PSEntryPoint);
	case ShaderType::GeometryShader: return String::ToWString(Desc.EntryPoints.GSEntryPoint);
	case ShaderType::HullShader:     return String::ToWString(Desc.EntryPoints.HSEntryPoint);
	case ShaderType::DomainShader:   return String::ToWString(Desc.EntryPoints.DSEntryPoint);
	default :
		ASSERT(false, "Unknown Shader Type : ShaderEntryPoint")
		return L"";
	}
}

string RenderingShader::GetShaderTarget(ShaderType Type) const
{
	switch (Type)
	{
	case ShaderType::VertexShader:	 return "vs_5_0";
	case ShaderType::PixelShader:	 return "ps_5_0";
	case ShaderType::GeometryShader: return "gs_5_0";
	case ShaderType::HullShader:     return "hs_5_0";
	case ShaderType::DomainShader:   return "ds_5_0";
	default:
		ASSERT(false, "Unknown Shader Type : ShaderTarget")
		return "";
	}
}

void RenderingShader::LoadShader(ShaderType InType)
{
	ID3DBlob * ShaderBlob;

	const wstring PreCompiledFilePath = Desc.PreCompiledShaderFileDirectory +  Path::GetFileNameWithoutExtension(Desc.ShaderFileName) + L"_" + GetEntryPoint(InType) + L".cso";

	if (Path::IsDirectoryExist(Desc.PreCompiledShaderFileDirectory) == false)
		Path::CreateFolders(Desc.PreCompiledShaderFileDirectory);

	if (Desc.bForceRecompile || Path::IsFileExist(PreCompiledFilePath) == false)
	{
		ShaderBlob = CompileShader(Desc.ShaderFileName, Desc.ShaderMacros , InType);
		std::ofstream outFile(PreCompiledFilePath, std::ios::binary);
		outFile.write((char*)ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize());
		outFile.close();
	}
	else
	{
		ShaderBlob = LoadPreCompiled(PreCompiledFilePath);
	}

	CHECK(SUCCEEDED(CreateShader(ShaderBlob, InType)));

	SAFE_RELEASE(ShaderBlob);
}

HRESULT RenderingShader::CreateShader(ID3DBlob* ShaderBlob, ShaderType InType)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	HRESULT Hr = 0;
	
	const void * BufferAddr = ShaderBlob->GetBufferPointer();
	const UINT BufferSize = ShaderBlob->GetBufferSize();
	if (InType == ShaderType::VertexShader)
	{
		Hr = Device->CreateVertexShader(BufferAddr, BufferSize, nullptr, &Pass.VertexShader);
		InitializeInputLayout(ShaderBlob);
	}
	else if (InType == ShaderType::HullShader)
		Hr = Device->CreateHullShader(BufferAddr, BufferSize, nullptr, &Pass.HullShader);
	else if (InType == ShaderType::DomainShader)
		Hr = Device->CreateDomainShader(BufferAddr, BufferSize, nullptr, &Pass.DomainShader);
	else if (InType == ShaderType::GeometryShader)
		Hr = Device->CreateGeometryShader(BufferAddr, BufferSize, nullptr, &Pass.GeometryShader);
	else if (InType == ShaderType::PixelShader)
		Hr = Device->CreatePixelShader(BufferAddr, BufferSize, nullptr, &Pass.PixelShader);
	else
		ASSERT(false, "Shader Type Not Supported")
	return Hr;
}

ID3DBlob* RenderingShader::CompileShader
(
	const wstring& InFileName,
	const D3D_SHADER_MACRO* InMacros,
	ShaderType InType
)
{
	ID3DBlob * ShaderBlob = nullptr;
	ID3DBlob * ErrorBlob = nullptr;
	
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/d3dcompile-constants
	int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR |
			   D3DCOMPILE_OPTIMIZATION_LEVEL3 |
			   D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
	Flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	const wstring & EntryPoint = GetEntryPoint(InType);
	const string & ShaderTarget = GetShaderTarget(InType);
	HRESULT Hr = D3DCompileFromFile(
		InFileName.c_str(),
		InMacros,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // HLSL내에서 #include 쓸 수 있게 해줌. custom ID3DInclude도 가능.
		String::ToString(EntryPoint).c_str(),
		ShaderTarget.c_str(),
		Flag,
		0,
		&ShaderBlob,
		&ErrorBlob
	);

	if (FAILED(Hr) && ErrorBlob != nullptr)
	{
		const char * const ErrMsg = static_cast<char *>(ErrorBlob->GetBufferPointer());
		SAFE_RELEASE(ErrorBlob);
		ASSERT(false, (String::ToString(InFileName) + " Failed to Compile :\n" + "<" + ErrMsg + ">").c_str())
		return nullptr;
	}
	if (FAILED(Hr) && ErrorBlob == nullptr)
	{
		SAFE_RELEASE(ErrorBlob);
		ASSERT(false, (String::ToString(InFileName) + " Failed to Compile : Maybe No File or Invalid EntryPoint").c_str())
		return nullptr;
	}
	SAFE_RELEASE(ErrorBlob);
	
	return ShaderBlob;
}

ID3DBlob * RenderingShader::LoadPreCompiled
(
	const wstring & InFilename
)
{
	ID3DBlob * ShaderBlob = nullptr;
	
	std::ifstream File(InFilename, std::ios::binary | std::ios::ate);
	if (File.is_open() == true)
	{
		std::streamsize FileSize = File.tellg();
		File.seekg(0, std::ios::beg);

		HRESULT Hr = D3DCreateBlob(static_cast<SIZE_T>(FileSize), &ShaderBlob);
		CHECK(SUCCEEDED(Hr));
		
		File.read((char*)ShaderBlob->GetBufferPointer(), FileSize);
		File.close();
	}
	else
	{
		ASSERT(false, "Failed to open cso File")
	}
	return ShaderBlob;
}

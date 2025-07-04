#include "framework.h"
#include <fstream>
#include "ComputeShader.h"

ComputeShaderDesc::ComputeShaderDesc(const ComputeShaderDesc& InDesc)
{
	ShaderName = InDesc.ShaderName;
	ShaderFileName = InDesc.ShaderFileName;
	ShaderMacros = InDesc.ShaderMacros;
	EntryPoint = InDesc.EntryPoint;
	NumThreadDimX = InDesc.NumThreadDimX;
	NumThreadDimY = InDesc.NumThreadDimY;
	NumThreadDimZ = InDesc.NumThreadDimZ;
	DispatchX = InDesc.DispatchX;
	DispatchY = InDesc.DispatchY;
	DispatchZ = InDesc.DispatchZ;
	SamplerStateNames = InDesc.SamplerStateNames;
	bForceRecompile = InDesc.bForceRecompile;
}

ComputeShaderDesc::ComputeShaderDesc(
	string InShaderName,
	wstring InShaderFileName,
	const vector<pair<string, string>> & InShaderMacros,
	wstring InEntryPoint,
	UINT InNumThreadDimX,
	UINT InNumThreadDimY,
	UINT InNumThreadDimZ,
	UINT InDispatchX,
	UINT InDispatchY,
	UINT InDispatchZ,
	const vector<tuple<int, ShaderType, string>>& InSamplerStateNames,
	bool InbForceRecompile
)
{
	ShaderName = InShaderName;
	ShaderFileName = InShaderFileName;
	ShaderMacros = InShaderMacros;
	EntryPoint = InEntryPoint;
	NumThreadDimX = InNumThreadDimX;
	NumThreadDimY = InNumThreadDimY;
	NumThreadDimZ = InNumThreadDimZ;
	DispatchX = InDispatchX;
	DispatchY = InDispatchY;
	DispatchZ = InDispatchZ;
	SamplerStateNames = InSamplerStateNames;
	bForceRecompile = InbForceRecompile;
}

ComputeShader::ComputeShader(const ComputeShaderDesc& InDesc)
	: Desc(InDesc)
{
	ShaderManager * const ShaderManagerInst = ShaderManager::Get();
	ASSERT(!Desc.ShaderFileName.empty(), "Shader Name Empty");
	Desc.ShaderFileName = W_SHADER_PATH + Desc.ShaderFileName;
	const wstring ShaderDirectory = Path::GetDirectoryName(Desc.ShaderFileName);
	Desc.PreCompiledShaderFileDirectory = ShaderDirectory + L"PreCompiled/";
	
	LoadShader();
	
	Pass.DispatchX = InDesc.DispatchX;
	Pass.DispatchY = InDesc.DispatchY;
	Pass.DispatchZ = InDesc.DispatchZ;
	
	for ( const auto & Item : Desc.SamplerStateNames)
	{
		int RegisterIndex = std::get<0>(Item);
		ShaderType TargetShader = std::get<1>(Item);
		const string & SamplerStateName = std::get<2>(Item);
		ID3D11SamplerState * SamplerState = ShaderManagerInst->GetSamplerState(SamplerStateName);
		CHECK(!!SamplerState);
		Pass.SamplerStates[RegisterIndex] = {SamplerState, TargetShader };
	}
}

ComputeShader::~ComputeShader()
{
	SAFE_RELEASE(Pass.Shader);
}

void ComputeShader::Recompile()
{
	const wstring PreCompiledFilePath = Desc.PreCompiledShaderFileDirectory +  Path::GetFileNameWithoutExtension(Desc.ShaderFileName) + L"_" + GetEntryPoint() + L".cso";
	
	if (Path::IsDirectoryExist(Desc.PreCompiledShaderFileDirectory) == false)
		Path::CreateFolders(Desc.PreCompiledShaderFileDirectory);
	
	ID3DBlob* ShaderBlob = CompileShader(Desc.ShaderFileName, Desc.ShaderMacros);
	std::ofstream outFile(PreCompiledFilePath, std::ios::binary);
	outFile.write((char*)ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize());
	outFile.close();

	SAFE_RELEASE(Pass.Shader);
	CHECK(SUCCEEDED(CreateShader(ShaderBlob, ShaderType::ComputeShader)));
	
	SAFE_RELEASE(ShaderBlob);
}

void ComputeShader::SetPass() const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->CSSetShader(this->Pass.Shader, nullptr, 0);
	for ( const auto & Pair : Pass.SamplerStates)
		DeviceContext->CSSetSamplers(Pair.first, 1, &Pair.second.SamplerState);
	for ( const auto & Pair : CBs)
		DeviceContext->CSSetConstantBuffers(Pair.first, 1, &Pair.second);
	for ( const auto & Pair : SRVs)
		DeviceContext->CSSetShaderResources(Pair.first, 1, &Pair.second);
	for ( const auto & Pair : UAVs)
		DeviceContext->CSSetUnorderedAccessViews(Pair.first, 1, &Pair.second, nullptr);
}

void ComputeShader::GetThreadDim(UINT& X, UINT& Y, UINT& Z) const
{
	X = this->Desc.NumThreadDimX;
	Y = this->Desc.NumThreadDimY;
	Z = this->Desc.NumThreadDimZ;
}

void ComputeShader::ClearPass()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	ID3D11UnorderedAccessView * NullUAV = nullptr;
	ID3D11ShaderResourceView * NullSRV = nullptr;
	ID3D11Buffer * NullBuffer = nullptr;
	ID3D11SamplerState * NullSampler = nullptr;

	for ( const auto & Pair : Pass.SamplerStates)
		DeviceContext->CSSetSamplers(Pair.first, 1, &NullSampler);
	for ( const auto & Pair : CBs)
		DeviceContext->CSSetConstantBuffers(Pair.first, 1, &NullBuffer);
	for ( const auto & Pair : SRVs)
		DeviceContext->CSSetShaderResources(Pair.first, 1, &NullSRV);
	for ( const auto & Pair : UAVs)
		DeviceContext->CSSetUnorderedAccessViews(Pair.first, 1, &NullUAV, nullptr);

	DeviceContext->CSSetShader(nullptr, nullptr, 0);
	CBs.clear();
	SRVs.clear();
	UAVs.clear();
}
void ComputeShader::Dispatch()
{
	ASSERT(Pass.DispatchX > 0 && Pass.DispatchY > 0 && Pass.DispatchZ > 0, "Use Dispatch(X, Y, Z)");
	SetPass();
	D3D::Get()->GetDeviceContext()->Dispatch(Pass.DispatchX, Pass.DispatchY, Pass.DispatchZ);
	ClearPass();
}
void ComputeShader::Dispatch(const UINT X, const UINT Y, const UINT Z)
{
	SetPass();
	D3D::Get()->GetDeviceContext()->Dispatch(X, Y, Z);
	ClearPass();
}

void ComputeShader::BindUAV(ID3D11UnorderedAccessView* InUAV, UINT SlotNum)
{
	UAVs[SlotNum] = InUAV;
}

void ComputeShader::BindSRV(ID3D11ShaderResourceView* InSRV, UINT SlotNum)
{
	SRVs[SlotNum] = InSRV;
}

void ComputeShader::BindCB(const BufferBase * InBuffer, UINT SlotNum)
{
	CBs[SlotNum] = InBuffer->GetBuffer();
}

void ComputeShader::BindCB(ID3D11Buffer* InConstantBuffer, UINT SlotNum)
{
	CBs[SlotNum] = InConstantBuffer;
}

void ComputeShader::LoadShader()
{
	ID3DBlob * ShaderBlob;
	const wstring PreCompiledFilePath = Desc.PreCompiledShaderFileDirectory +  Path::GetFileNameWithoutExtension(Desc.ShaderFileName) + L"_" + GetEntryPoint() + L".cso";
	
	if (Path::IsDirectoryExist(Desc.PreCompiledShaderFileDirectory) == false)
		Path::CreateFolders(Desc.PreCompiledShaderFileDirectory);

	if (Desc.bForceRecompile || Path::IsFileExist(PreCompiledFilePath) == false)
	{
		ShaderBlob = CompileShader(Desc.ShaderFileName, Desc.ShaderMacros);
		std::ofstream outFile(PreCompiledFilePath, std::ios::binary);
		outFile.write((char*)ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize());
		outFile.close();
	}
	else
	{
		ShaderBlob = LoadPreCompiled(PreCompiledFilePath);
	}

	CHECK(SUCCEEDED(CreateShader(ShaderBlob, ShaderType::None)));

	SAFE_RELEASE(ShaderBlob);
}

wstring ComputeShader::GetEntryPoint(ShaderType Type) const
{
	return this->Desc.EntryPoint;
}

string ComputeShader::GetShaderTarget(ShaderType Type) const
{
	return "cs_5_0";
}

ID3DBlob* ComputeShader::CompileShader
(
	const wstring& InFileName,
	const vector<pair<string, string>> & InMacros,
	ShaderType InType
)
{
	vector<D3D_SHADER_MACRO> ShaderMacros;
	for (const pair<string, string> & Macro : InMacros)
	{
		ShaderMacros.push_back({Macro.first.c_str(), Macro.second.c_str()});
	}
	ShaderMacros.push_back({nullptr, nullptr});
	
	ID3DBlob * ShaderBlob = nullptr;
	ID3DBlob * ErrorBlob = nullptr;
	int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR |
		   D3DCOMPILE_OPTIMIZATION_LEVEL3 |
		   D3DCOMPILE_WARNINGS_ARE_ERRORS;
	string EntryPoint = String::ToString(GetEntryPoint());
	string TargetName = GetShaderTarget();
	HRESULT Hr = D3DCompileFromFile(
		InFileName.c_str(),
		ShaderMacros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // HLSL내에서 #include 쓸 수 있게 해줌. custom ID3DInclude도 가능.
		EntryPoint.c_str(),
		TargetName.c_str(),
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

ID3DBlob* ComputeShader::LoadPreCompiled(const wstring& InFilename)
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

HRESULT ComputeShader::CreateShader(ID3DBlob* ShaderBlob, ShaderType InType)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	const void * BufferAddr = ShaderBlob->GetBufferPointer();
	const UINT BufferSize = ShaderBlob->GetBufferSize();

	HRESULT Hr = Device->CreateComputeShader(BufferAddr, BufferSize, nullptr, &Pass.Shader);

	return Hr;
}


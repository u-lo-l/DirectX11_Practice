#include "Framework.h"
#include "Shader.h"

Shader::Shader(const wstring& File)
	: FileName(W_SHADER_PATH + File)
{
	InitialStateBlock = new StateBlock();

	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();
	Context->RSGetState(&InitialStateBlock->RSRasterizerState);
	Context->OMGetBlendState(&InitialStateBlock->OMBlendState, InitialStateBlock->OMBlendFactor, &InitialStateBlock->OMSampleMask);
	Context->OMGetDepthStencilState(&InitialStateBlock->OMDepthStencilState, &InitialStateBlock->OMStencilRef);

	CreateEffect();
}

Shader::~Shader()
{
	for (Technique & temp : Techniques)
	{
		for (Pass & pass : temp.Passes)
		{
			SAFE_RELEASE(pass.InputLayout);
		}
	}

	SAFE_DELETE(InitialStateBlock);
	SAFE_RELEASE(Effect);
}



void Shader::CreateEffect()
{
	ID3DBlob * fxBlob = nullptr;
	ID3DBlob * error = nullptr;
	constexpr INT flag = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

//https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-programming-guide-effects-compile
	/*
	 * Compile an Effect
	 * Shader파일을 컴파일하여 그 정보를 ID3DBlob객체에 저장한다.(여기선 fxBlob)
	 */
	HRESULT hr = D3DCompileFromFile(
		FileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		nullptr,
		"fx_5_0",
		flag, 
		0,
		&fxBlob,
		&error);

	if (FAILED(hr))
	{
		if (error != nullptr)
		{
			// ReSharper disable once CppUseAuto
			const char * str = static_cast<const char*>(error->GetBufferPointer());
			MessageBoxA(nullptr, str, "Shader Error", MB_OK);
		}

		ASSERT(false, "Fx File not found");
	}

//https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3dx11createeffectfrommemory
	/*
	 * Create an Effect
	 * ID3DBlob객체(바이너리데이터)로부터 Effect 객체를 생성하는 함수다.
	 */
	hr = D3DX11CreateEffectFromMemory(
		fxBlob->GetBufferPointer(),
		fxBlob->GetBufferSize(),
		0,
		D3D::Get()->GetDevice(),
		&Effect);
	CHECK(hr >= 0);
	CHECK(Effect->GetDesc(&EffectDesc) >= 0);


	/*
	 * Effect는 여러 Technique들을 가질 수 있고, 각각의 technique들을 다루는 부분이다.
	 * 또한 하나의 Technique에는 여러 Pass가 포함된다.
	 * "Pass"를 표현하는 객체를 반환 받아야하기 떄문에, index 혹은 name으로 technique를 얻고,
	 * technique에서 pass를 얻어야한다.
	 *
	 *	struct D3DX11_EFFECT_DESC
		{
		    uint32_t    ConstantBuffers;        // Number of constant buffers in this effect
		    uint32_t    GlobalVariables;        // Number of global variables in this effect
		    uint32_t    InterfaceVariables;     // Number of global interfaces in this effect
		    uint32_t    Techniques;             // Number of techniques in this effect
		    uint32_t    Groups;                 // Number of groups in this effect
		};
		EffectDesc구조체는 다음 값들을 갖는다. 여기서 Technique, ConstantBuffer, GlobalVariable만 확인하는거다.
	 */
	Techniques.reserve(EffectDesc.Techniques);
	for (UINT t = 0; t < EffectDesc.Techniques; t++)
	{
		Technique technique;
		technique.ITechnique = Effect->GetTechniqueByIndex(t);
		CHECK(technique.ITechnique->GetDesc(&technique.Desc) >= 0);

		technique.Passes.reserve(technique.Desc.Passes);
		for (UINT p = 0; p < technique.Desc.Passes; p++)
		{
			Pass pass;
			pass.IPass = technique.ITechnique->GetPassByIndex(p);
			pass.CheckPassValid();

			pass.SignatureDescs.reserve(pass.EffectVsDesc.NumInputSignatureEntries);
			for (UINT s = 0; s < pass.EffectVsDesc.NumInputSignatureEntries; s++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;

				const HRESULT Hresult = pass.PassVsDesc.pShaderVariable->GetInputSignatureElementDesc(pass.PassVsDesc.ShaderIndex, s, &desc);
				CHECK(Hresult >= 0);

				pass.SignatureDescs.emplace_back(desc);
			}

			pass.InputLayout = Shader::CreateInputLayout(fxBlob, &pass.EffectVsDesc, pass.SignatureDescs);
			pass.StateBlock = InitialStateBlock;

			technique.Passes.emplace_back(pass);
		}
		Techniques.emplace_back(technique);
	}

	// Effect 객체의 상수버퍼를 가져온다.
	// 상수버퍼란, 쉐이더 내에서 변경되지 않는 데이터를 전달하기 위해 사용되는 버퍼이다.
	// 변환 행렬, 뷰포트 정보등이 이에 포함된다.
	/*for (UINT i = 0; i < EffectDesc.ConstantBuffers; i++)
	{
		ID3DX11EffectConstantBuffer * iBuffer = Effect->GetConstantBufferByIndex(i);
		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		CHECK(iBuffer->GetDesc(&vDesc) >= 0);
	}

	for (UINT i = 0; i < EffectDesc.GlobalVariables; i++)
	{
		ID3DX11EffectVariable * iVariable = Effect->GetVariableByIndex(i);
		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		CHECK(iVariable->GetDesc(&vDesc) >= 0);
	}*/

	SAFE_RELEASE(fxBlob);
}

ID3D11InputLayout * Shader::CreateInputLayout
(
	ID3DBlob * FxBlob,
	const D3DX11_EFFECT_SHADER_DESC * EffectVsDesc,
	const vector<D3D11_SIGNATURE_PARAMETER_DESC> & Params
)
{
	// Step 01 : inputLayoutDecs생성
	// Shader input signature를 바탕으로 D3D11_INPUT_ELEMENT_DESC를 초기화한다.
	// D3D11_INPUT_ELEMENT_DESC : DX에서 InputLayout을 정의하는 핵심이되는 구조체이다.
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
	inputLayoutDescs.reserve(Params.size());
	for (const D3D11_SIGNATURE_PARAMETER_DESC & paramDesc : Params)
	{
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string name = paramDesc.SemanticName;
		transform(name.begin(), name.end(), name.begin(), toupper);

		if (name == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		if ((name.substr(0, 3) == "SV_") == false)
			inputLayoutDescs.push_back(elementDesc);
	}
	if (inputLayoutDescs.size() == 0)
		return nullptr;

	const void* pCode = EffectVsDesc->pBytecode;
	const UINT pCodeSize = EffectVsDesc->BytecodeLength;

	ID3D11InputLayout * inputLayout = nullptr;
	/*
	 * D3D11_INPUT_ELEMENT_DESC : IA 단계의 데이터 형식
	 */
	const HRESULT Hresult = D3D::Get()->GetDevice()->CreateInputLayout
	(
		&inputLayoutDescs[0], // IA 단계의 데이터 형식 배열
		inputLayoutDescs.size(),
		pCode, // 컴파일된 쉐이더
		pCodeSize, // 컴파일된 쉐이더의 크기
		&inputLayout // [out]생성된 InputLayout
	);
	CHECK(Hresult >= 0);

	return inputLayout;
}

void Shader::Pass::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->Draw(VertexCount, StartVertexLocation);
	EndDraw();
}

void Shader::Pass::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	EndDraw();
}

void Shader::Pass::BeginDraw()
{
	CHECK(IPass->ComputeStateBlockMask(&StateBlockMask) >= 0);
	D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout);
	CHECK(IPass->Apply(0, D3D::Get()->GetDeviceContext()) >= 0);
}

void Shader::Pass::EndDraw() const
{
	ID3D11DeviceContext* Context = D3D::Get()->GetDeviceContext();

	if (StateBlockMask.RSRasterizerState == 1)
		Context->RSSetState(StateBlock->RSRasterizerState);

	if (StateBlockMask.OMDepthStencilState == 1)
		Context->OMSetDepthStencilState(StateBlock->OMDepthStencilState, StateBlock->OMStencilRef);

	if (StateBlockMask.OMBlendState == 1)
		Context->OMSetBlendState(StateBlock->OMBlendState, StateBlock->OMBlendFactor, StateBlock->OMSampleMask);

	Context->HSSetShader(nullptr, nullptr, 0);
	Context->DSSetShader(nullptr, nullptr, 0);
	Context->GSSetShader(nullptr, nullptr, 0);
}

void Shader::Pass::Dispatch(UINT X, UINT Y, UINT Z) const
{
	ID3D11DeviceContext* Context = D3D::Get()->GetDeviceContext();

	CHECK(IPass->Apply(0, Context) >= 0);
	Context->Dispatch(X, Y, Z);


	ID3D11ShaderResourceView* Null[1] = { nullptr };
	Context->CSSetShaderResources(0, 1, Null);

	ID3D11UnorderedAccessView* NullUav[1] = { nullptr };
	Context->CSSetUnorderedAccessViews(0, 1, NullUav, nullptr);

	Context->CSSetShader(nullptr, nullptr, 0);
}

void Shader::Pass::CheckPassValid()
{
	CHECK(IPass->GetDesc(&Desc) >= 0);
	CHECK(IPass->GetVertexShaderDesc(&PassVsDesc) >= 0);
	CHECK(PassVsDesc.pShaderVariable->GetShaderDesc(PassVsDesc.ShaderIndex, &EffectVsDesc) >= 0);
}

void Shader::Technique::Draw(UINT Pass, UINT VertexCount, UINT StartVertexLocation)
{
	Passes[Pass].Draw(VertexCount, StartVertexLocation);
}

void Shader::Technique::DrawIndexed(UINT Pass, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	Passes[Pass].DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void Shader::Technique::Dispatch(UINT Pass, UINT X, UINT Y, UINT Z) const
{
	Passes[Pass].Dispatch(X, Y, Z);
}

void Shader::Draw(UINT TechniqueIndex, UINT PassIndex, UINT VertexCount, UINT StartVertexLocation)
{
	Techniques[TechniqueIndex].Draw(PassIndex, VertexCount, StartVertexLocation);
}

void Shader::Dispatch(UINT TechniqueIndex, UINT PassIndex, UINT X, UINT Y, UINT Z) const
{
	Techniques[TechniqueIndex].Dispatch(PassIndex, X, Y, Z);

}

// BaseVertexLocation : StartIndexLocation이 BaseVertexLocation을 기준으로 한다.
void Shader::DrawIndexed(UINT TechniqueIndex, UINT Pass, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	Techniques[TechniqueIndex].DrawIndexed(Pass, IndexCount, StartIndexLocation, BaseVertexLocation);
}

//ID3DX11EffectVariable* Shader::Variable(const string & Name)  const
//{
//	return Effect->GetVariableByName(Name.c_str());
//}

ID3DX11EffectScalarVariable* Shader::AsScalar(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsScalar();
}

ID3DX11EffectVectorVariable* Shader::AsVector(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsVector();
}

ID3DX11EffectMatrixVariable* Shader::AsMatrix(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsMatrix();
}

ID3DX11EffectStringVariable* Shader::AsString(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsString();
}

ID3DX11EffectShaderResourceVariable* Shader::AsSRV(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsShaderResource();
}

ID3DX11EffectRenderTargetViewVariable* Shader::AsRTV(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsRenderTargetView();
}

ID3DX11EffectDepthStencilViewVariable* Shader::AsDSV(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsDepthStencilView();
}

ID3DX11EffectConstantBuffer* Shader::AsConstantBuffer(const string & Name) const
{
	return Effect->GetConstantBufferByName(Name.c_str());
}

ID3DX11EffectShaderVariable* Shader::AsShader(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsShader();
}

ID3DX11EffectBlendVariable* Shader::AsBlend(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsBlend();
}

ID3DX11EffectDepthStencilVariable* Shader::AsDepthStencil(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsDepthStencil();
}

ID3DX11EffectRasterizerVariable* Shader::AsRasterizer(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsRasterizer();
}

ID3DX11EffectSamplerVariable* Shader::AsSampler(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsSampler();
}

ID3DX11EffectUnorderedAccessViewVariable* Shader::AsUAV(const string & Name) const
{
	return Effect->GetVariableByName(Name.c_str())->AsUnorderedAccessView();
}
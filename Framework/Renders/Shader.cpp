#include "Framework.h"
#include "Shader.h"

Shader::Shader(const wstring& File)
	: FileName(L"../../_Shaders/" + File)
{
	InitialStateBlock = new StateBlock();
	{
		D3D::Get()->GetDeviceContext()->RSGetState(&InitialStateBlock->RSRasterizerState);
		D3D::Get()->GetDeviceContext()->OMGetBlendState(&InitialStateBlock->OMBlendState, InitialStateBlock->OMBlendFactor, &InitialStateBlock->OMSampleMask);
		D3D::Get()->GetDeviceContext()->OMGetDepthStencilState(&InitialStateBlock->OMDepthStencilState, &InitialStateBlock->OMStencilRef);
	}

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

		assert(false && "Fx File not found");
	}

	hr = D3DX11CreateEffectFromMemory(
		fxBlob->GetBufferPointer(),
		fxBlob->GetBufferSize(),
		0,
		D3D::Get()->GetDevice(),
		&Effect);
	CHECK(hr);


	CHECK(Effect->GetDesc(&EffectDesc));
	for (UINT t = 0; t < EffectDesc.Techniques; t++)
	{
		Technique technique;
		technique.ITechnique = Effect->GetTechniqueByIndex(t);
		CHECK(technique.ITechnique->GetDesc(&technique.Desc));

		string temp;

		for (UINT p = 0; p < technique.Desc.Passes; p++)
		{
			Pass pass;
			pass.IPass = technique.ITechnique->GetPassByIndex(p);
			CHECK(pass.IPass->GetDesc(&pass.Desc));
			CHECK(pass.IPass->GetDesc(&pass.Desc));
			CHECK(pass.IPass->GetVertexShaderDesc(&pass.PassVsDesc));
			CHECK(pass.PassVsDesc.pShaderVariable->GetShaderDesc(pass.PassVsDesc.ShaderIndex, &pass.EffectVsDesc));

			for (UINT s = 0; s < pass.EffectVsDesc.NumInputSignatureEntries; s++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;

				const HRESULT Hresult = pass.PassVsDesc.pShaderVariable->GetInputSignatureElementDesc(pass.PassVsDesc.ShaderIndex, s, &desc);
				CHECK(Hresult);

				pass.SignatureDescs.push_back(desc);
			}

			pass.InputLayout = Shader::CreateInputLayout(fxBlob, &pass.EffectVsDesc, pass.SignatureDescs);
			pass.StateBlock = InitialStateBlock;

			technique.Passes.push_back(pass);
		}

		Techniques.push_back(technique);
	}

	for (UINT i = 0; i < EffectDesc.ConstantBuffers; i++)
	{
		ID3DX11EffectConstantBuffer * iBuffer = Effect->GetConstantBufferByIndex(i);

		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		CHECK(iBuffer->GetDesc(&vDesc));

		//int a = 0;
	}

	for (UINT i = 0; i < EffectDesc.GlobalVariables; i++)
	{
		ID3DX11EffectVariable * iVariable = Effect->GetVariableByIndex(i);
		D3DX11_EFFECT_VARIABLE_DESC vDesc;
		CHECK(iVariable->GetDesc(&vDesc));

		//int a = 0;
	}

	SAFE_RELEASE(fxBlob);
}

ID3D11InputLayout * Shader::CreateInputLayout(
	ID3DBlob * FxBlob,
	const D3DX11_EFFECT_SHADER_DESC * EffectVsDesc,
	const vector<D3D11_SIGNATURE_PARAMETER_DESC>& Params )
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
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
			inputLayoutDesc.push_back(elementDesc);
	}


	const void* pCode = EffectVsDesc->pBytecode;
	const UINT pCodeSize = EffectVsDesc->BytecodeLength;

	if (inputLayoutDesc.size() > 0)
	{
		ID3D11InputLayout* inputLayout = nullptr;
		const HRESULT Hresult = D3D::Get()->GetDevice()->CreateInputLayout
		(
			&inputLayoutDesc[0]
			, inputLayoutDesc.size()
			, pCode
			, pCodeSize
			, &inputLayout
		);
		CHECK(Hresult);

		return inputLayout;
	}

	return nullptr;
}

void Shader::Pass::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	BeginDraw();
	{
		D3D::Get()->GetDeviceContext()->Draw(VertexCount, StartVertexLocation);
	}
	EndDraw();
}

void Shader::Pass::BeginDraw()
{
	CHECK(IPass->ComputeStateBlockMask(&StateBlockMask));

	D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout);
	CHECK(IPass->Apply(0, D3D::Get()->GetDeviceContext()));
}

void Shader::Pass::EndDraw() const
{
	if (StateBlockMask.RSRasterizerState == 1)
		D3D::Get()->GetDeviceContext()->RSSetState(StateBlock->RSRasterizerState);

	if (StateBlockMask.OMDepthStencilState == 1)
		D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(StateBlock->OMDepthStencilState, StateBlock->OMStencilRef);

	if (StateBlockMask.OMBlendState == 1)
		D3D::Get()->GetDeviceContext()->OMSetBlendState(StateBlock->OMBlendState, StateBlock->OMBlendFactor, StateBlock->OMSampleMask);

	D3D::Get()->GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
	D3D::Get()->GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
	D3D::Get()->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
}

void Shader::Pass::Dispatch(UINT X, UINT Y, UINT Z) const
{
	CHECK(IPass->Apply(0, D3D::Get()->GetDeviceContext()));
	D3D::Get()->GetDeviceContext()->Dispatch(X, Y, Z);


	ID3D11ShaderResourceView * Null[1] = { nullptr };
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(0, 1, Null);

	ID3D11UnorderedAccessView * NullUav[1] = { nullptr };
	D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, NullUav, nullptr);

	D3D::Get()->GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);
}

void Shader::Technique::Draw(UINT Pass, UINT VertexCount, UINT StartVertexLocation)
{
	Passes[Pass].Draw(VertexCount, StartVertexLocation);
}

void Shader::Technique::Dispatch(UINT Pass, UINT X, UINT Y, UINT Z) const
{
	Passes[Pass].Dispatch(X, Y, Z);
}

void Shader::Draw(UINT TechniqueIndex, UINT PassIndex, UINT VertexCount, UINT StartVertexLocation)
{
	Techniques[TechniqueIndex].Passes[PassIndex].Draw(VertexCount, StartVertexLocation);
}

void Shader::Dispatch(UINT TechniqueIndex, UINT PassIndex, UINT X, UINT Y, UINT Z) const
{
	Techniques[TechniqueIndex].Passes[PassIndex].Dispatch(X, Y, Z);
}

ID3DX11EffectVariable* Shader::Variable(const string & Name)  const
{
	return Effect->GetVariableByName(Name.c_str());
}

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
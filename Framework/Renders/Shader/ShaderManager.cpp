#include "framework.h"
#include "ShaderManager.h"

ShaderManager * ShaderManager::Instance = nullptr;

void ShaderManager::Create()
{
	ASSERT(Instance == nullptr, "Instance Already Exist");
	Instance = new ShaderManager();
	
	Instance->InitSamplerStates();
	Instance->InitDepthStencilStates();
	Instance->InitBlendStates();
	Instance->InitRasterizerStates();
	Instance->InitRenderingShaders();
	Instance->InitComputeShaders();
}

void ShaderManager::Destroy()
{
	SAFE_DELETE(Instance);
}

ShaderManager * ShaderManager::Get()
{
	ASSERT(Instance != nullptr, "Instance Not Exist")
	return Instance;
}

RenderingShader * ShaderManager::GetRenderShader(const string& InName)
{
	const auto & It = RenderShaderMap.find(InName);
	return It == RenderShaderMap.cend() ? nullptr : It->second;
}

ComputeShader * ShaderManager::GetComputeShader(const string& InName)
{
	const auto & It = ComputeShaderMap.find(InName);
	return It == ComputeShaderMap.cend() ? nullptr : It->second;
}

void ShaderManager::AddShader(const wstring& InName, ShaderBase* InShader)
{
	RenderingShader * RenderShader = dynamic_cast<RenderingShader *>(InShader);
	if (RenderShader != nullptr)
	{
		AddRenderShader(InName, RenderShader);
		return ;
	}
	ComputeShader * Compute = dynamic_cast<ComputeShader *>(InShader);
	if (Compute != nullptr)
	{
		AddComputeShader(InName, Compute);
		return ;
	}
	ASSERT(false, "Unknown Shadear Type");
}

void ShaderManager::AddSamplerState(const string& InName, ID3D11SamplerState* InSamplerState)
{
	const auto & It = SamplerStateTable.find(InName);
	ASSERT(It == SamplerStateTable.cend(), "Sampler State Already Exist");

	SamplerStateTable[InName] = InSamplerState;
}

void ShaderManager::AddRasterizerState(const string& InName, ID3D11RasterizerState* InRasterState)
{
	const auto & It = RasterizerStateTable.find(InName);
	ASSERT(It == RasterizerStateTable.cend(), "Sampler State Already Exist");

	RasterizerStateTable[InName] = InRasterState;
}

void ShaderManager::AddDepthStencilState(const string& InName, ID3D11DepthStencilState* InDepthStencilState)
{
	const auto & It = DepthStencilStateTable.find(InName);
	ASSERT(It == DepthStencilStateTable.cend(), "Sampler State Already Exist");

	DepthStencilStateTable[InName] = InDepthStencilState;
}

void ShaderManager::AddBlendState(const string& InName, ID3D11BlendState* InBlendState)
{
	const auto & It = BlendStateTable.find(InName);
	ASSERT(It == BlendStateTable.cend(), "Sampler State Already Exist");

	BlendStateTable[InName] = InBlendState;
}

ID3D11BlendState* ShaderManager::GetBlendState(const string& InName)
{
	const auto & It = BlendStateTable.find(InName);
	if (It == BlendStateTable.cend())
		return nullptr;
	return It->second;
}

ID3D11RasterizerState* ShaderManager::GetRasterizerState(const string& InName)
{
	const auto & It = RasterizerStateTable.find(InName);
		if (It == RasterizerStateTable.cend())
		return nullptr;
	return It->second;
}

ID3D11SamplerState* ShaderManager::GetSamplerState(const string& InName)
{
	const auto & It = SamplerStateTable.find(InName);
	if (It == SamplerStateTable.cend())
		return nullptr;
	return It->second;}

ID3D11DepthStencilState* ShaderManager::GetDepthStencilState(const string& InName)
{
	const auto & It = DepthStencilStateTable.find(InName);
	if (It == DepthStencilStateTable.cend())
		return nullptr;
	return It->second;}

vector<string> ShaderManager::GetBlendStateNames() const
{
	vector<string> Names;
	for (const auto & It : BlendStateTable)
		Names.push_back(It.first);
	return Names;
}

vector<string> ShaderManager::GetRasterizerStateNames() const
{
	vector<string> Names;
	for (const auto & It : RasterizerStateTable)
		Names.push_back(It.first);
	return Names;
}

vector<string> ShaderManager::GetDepthStencilStateNames() const
{
	vector<string> Names;
	for (const auto & It : DepthStencilStateTable)
		Names.push_back(It.first);
	return Names;
}

vector<string> ShaderManager::GetSamplerStateNames() const
{
	vector<string> Names;
	for (const auto & It : SamplerStateTable)
		Names.push_back(It.first);
	return Names;
}

vector<string> ShaderManager::GetRenderingShaderNames() const
{
	vector<string> Names;
	for (const auto & It : RenderShaderMap)
		Names.push_back(It.first);
	return Names;
}

vector<string> ShaderManager::GetComputeShaderNames() const
{
	vector<string> Names;
	for (const auto & It : ComputeShaderMap)
		Names.push_back(It.first);
	return Names;
}

bool ShaderManager::IsValidRenderingShaderName(const string& InName)
{
	return RenderShaderMap.find(InName) != RenderShaderMap.cend();
}

void ShaderManager::InitSamplerStates()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	ID3D11SamplerState * Point_Wrap = nullptr;
	ID3D11SamplerState * Point_Clamp = nullptr;
	ID3D11SamplerState * Linear_Wrap = nullptr;
	ID3D11SamplerState * Linear_Clamp = nullptr;
	ID3D11SamplerState * Anisotropic_Wrap = nullptr;
	ID3D11SamplerState * Anisotropic_Clamp = nullptr;

	D3D11_SAMPLER_DESC SamplerDesc = {};
	memset(&SamplerDesc, 0, sizeof(SamplerDesc));
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MaxAnisotropy = 16;
	
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Point_Wrap)));

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Point_Clamp)));

	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Linear_Wrap)));

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Linear_Clamp)));

	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Anisotropic_Wrap)));

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK(SUCCEEDED(Device->CreateSamplerState(&SamplerDesc, &Anisotropic_Clamp)));
	
	Instance->AddSamplerState("Point_Wrap", Point_Wrap);
	Instance->AddSamplerState("Point_Clamp", Point_Clamp);
	Instance->AddSamplerState("Linear_Wrap", Linear_Wrap);
	Instance->AddSamplerState("Linear_Clamp", Linear_Clamp);
	Instance->AddSamplerState("Anisotropic_Wrap", Anisotropic_Wrap);
	Instance->AddSamplerState("Anisotropic_Clamp", Anisotropic_Clamp);
}

void ShaderManager::InitDepthStencilStates()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	ID3D11DepthStencilState * NoDepth;
	ID3D11DepthStencilState * Default;
	ID3D11DepthStencilState * Particle;

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};

	{
		memset(&DepthStencilDesc, 0, sizeof(DepthStencilDesc));
		DepthStencilDesc.DepthEnable = false;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.StencilEnable = false;

		CHECK(SUCCEEDED(Device->CreateDepthStencilState(&DepthStencilDesc, &NoDepth)));
	}

	{
		memset(&DepthStencilDesc, 0, sizeof(DepthStencilDesc));
		DepthStencilDesc.DepthEnable = true;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		DepthStencilDesc.StencilEnable = false;

		CHECK(SUCCEEDED(Device->CreateDepthStencilState(&DepthStencilDesc, &Default)));
	}
	
	{
		memset(&DepthStencilDesc, 0, sizeof(DepthStencilDesc));
		DepthStencilDesc.DepthEnable = true;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		DepthStencilDesc.StencilEnable = false;

		CHECK(SUCCEEDED(Device->CreateDepthStencilState(&DepthStencilDesc, &Particle)));
	}
	
	Instance->AddDepthStencilState("NoDepth", NoDepth);
	Instance->AddDepthStencilState("Default", Default);
	Instance->AddDepthStencilState("Particle", Particle);
}

void ShaderManager::InitBlendStates()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	ID3D11BlendState * Opaque;
	ID3D11BlendState * Transparent;
	ID3D11BlendState * Discard;
	ID3D11BlendState * Additive;
	ID3D11BlendState * AlphaCoverage;
	D3D11_BLEND_DESC BlendDesc = {};

	{
		memset(&BlendDesc, 0, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.IndependentBlendEnable = false;
	
		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CHECK(SUCCEEDED(Device->CreateBlendState(&BlendDesc, &Opaque)));
	}

	{
		memset(&BlendDesc, 0, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = true;
		BlendDesc.IndependentBlendEnable = false;
	
		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CHECK(SUCCEEDED(Device->CreateBlendState(&BlendDesc, &Transparent)));
	}
	{
		memset(&BlendDesc, 0, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.IndependentBlendEnable = false;
	
		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CHECK(SUCCEEDED(Device->CreateBlendState(&BlendDesc, &Discard)));
	}
	{
		memset(&BlendDesc, 0, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.IndependentBlendEnable = false;
	
		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CHECK(SUCCEEDED(Device->CreateBlendState(&BlendDesc, &Additive)));
	}

	{
		memset(&BlendDesc, 0, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = true; // 멀티샘플링 기반 알파 투명도 지원
		BlendDesc.IndependentBlendEnable = false;

		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		CHECK(SUCCEEDED(Device->CreateBlendState(&BlendDesc, &AlphaCoverage)));
	}
	
	Instance->AddBlendState("Opaque", Opaque);
	Instance->AddBlendState("Transparent", Transparent);
	Instance->AddBlendState("Discard", Discard);
	Instance->AddBlendState("Additive", Additive);
	Instance->AddBlendState("AlphaCoverage", AlphaCoverage);
}

void ShaderManager::InitRasterizerStates()
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	ID3D11RasterizerState * WireFrame;
	ID3D11RasterizerState * WireFrame_CullFront;
	ID3D11RasterizerState * WireFrame_NoCull;
	ID3D11RasterizerState * Solid;
	ID3D11RasterizerState * Solid_CullFront;
	ID3D11RasterizerState * Solid_NoCull;
	ID3D11RasterizerState * Solid_CCW;

	
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	{
		RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &WireFrame_NoCull)));
		RasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &WireFrame_CullFront)));
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &WireFrame)));
	}
	{
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &Solid_NoCull)));
		RasterizerDesc.CullMode = D3D11_CULL_FRONT;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &Solid_CullFront)));
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &Solid)));
		RasterizerDesc.FrontCounterClockwise = true;
		CHECK(SUCCEEDED(Device->CreateRasterizerState(&RasterizerDesc, &Solid_CCW)));
	}

	Instance->AddRasterizerState("WireFrame", WireFrame);
	Instance->AddRasterizerState("WireFrame_CullFront", WireFrame_CullFront);
	Instance->AddRasterizerState("WireFrame_NoCull", WireFrame_NoCull);
	Instance->AddRasterizerState("Solid", Solid);
	Instance->AddRasterizerState("Solid_CullFront", Solid_CullFront);
	Instance->AddRasterizerState("Solid_NoCull", Solid_NoCull);
	Instance->AddRasterizerState("Solid_CCW", Solid_CCW);
}

void ShaderManager::InitRenderingShaders()
{
	/**
	 * - RENDERER
	 * -- Terrain
	 * -- Ocean
	 * -- Foliage
	 * -- SkeletaMesh
	 * -- StaticMesh
	 * -- Particle
	 * -- Sprite
	 * -- Line
	 */
	RenderingShaderDesc Desc;
	{
		Desc.ShaderName = "SkeletalMesh";
		Desc.ShaderFileName = L"Mesh/SkeletalMesh.hlsl";
		Desc.pInputLayoutElements = &(VertexSkeletalMesh::GetVertexInputLayoutElements());
		Desc.Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Desc.ShaderMacros = nullptr;
		Desc.TargetShaderType = ShaderType::VP;
		Desc.RasterizerStateName = "Solid";
		Desc.BlendStateName = "Discard";
		Desc.DepthStencilStateName = "Default";
		Desc.SamplerStateNames.push_back({0, ShaderType::PixelShader, "Anisotropic_Wrap"});
		Desc.bForceRecompile = true;
		Instance->RenderShaderMap[Desc.ShaderName.c_str()] = new RenderingShader(Desc);;
	}
	{
		Desc.ShaderName = "StaticMesh";
		Desc.ShaderFileName = L"Mesh/StaticMesh.hlsl";
		Desc.pInputLayoutElements = &(VertexStaticMesh::GetVertexInputLayoutElements());
		Desc.Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Desc.ShaderMacros = nullptr;
		Desc.TargetShaderType = ShaderType::VP;
		Desc.RasterizerStateName = "Solid";
		Desc.BlendStateName = "Transparent";
		Desc.DepthStencilStateName = "Default";
		Desc.SamplerStateNames.push_back({0, ShaderType::PixelShader, "Linear_Wrap"});
		Desc.bForceRecompile = true;
		Instance->RenderShaderMap[Desc.ShaderName.c_str()] = new RenderingShader(Desc);;
	}
	{
		vector<D3D_SHADER_MACRO> Defines {
			{"TYPE01", ""},
			{nullptr, nullptr}
		};
		Desc.ShaderName = "Terrain";
		Desc.ShaderFileName = L"Terrain/TerrainCellInstance.hlsl";
		Desc.pInputLayoutElements = &(VertexTerrainCell::GetVertexInputLayoutElements());
		Desc.Topology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		Desc.ShaderMacros = Defines.data();
		Desc.TargetShaderType = ShaderType::VHDP;
		Desc.RasterizerStateName = "Solid";
		Desc.BlendStateName = "Opaque";
		Desc.DepthStencilStateName = "Default";
		Desc.SamplerStateNames.push_back({0, ShaderType::VDP, "Linear_Wrap"});
		Desc.SamplerStateNames.push_back({1, ShaderType::VDP, "Linear_Clamp"});
		Desc.bForceRecompile = true;
		Instance->RenderShaderMap[Desc.ShaderName.c_str()] = new RenderingShader(Desc);;
	}
	{
		// Foliage
	}
	{
		// Particle
	}
	
	ComputeShader * KeyFrame = nullptr;
	{
		// Animation
	}
}

void ShaderManager::InitComputeShaders()
{
	/**
	 * - COMPUTE
	 * -- KeyFrame
	 * -- NormalMapCreator
	 */
	ComputeShaderDesc Desc;
	{
		Desc = {};
		const vector<D3D_SHADER_MACRO> Defines{
			{"THREAD_X", "32"},
			{"THREAD_Y", "32"},
			{nullptr, nullptr}
		};
		Desc.ShaderName = "NormalMapCreator";
		Desc.ShaderFileName = L"ComputeShader/ComputeNormalTangentMap.hlsl";
		Desc.PreCompiledShaderFileDirectory = L"";
		Desc.ShaderMacros = Defines.data();
		Desc.EntryPoint = L"CSMain";
		Desc.NumThreadDimX = 16;
		Desc.NumThreadDimY = 16;
		Desc.NumThreadDimZ = 1;
		Desc.DispatchX = 0;
		Desc.DispatchY = 0;
		Desc.DispatchZ = 0;
		Desc.SamplerStateNames = {};
		Desc.bForceRecompile = true;
		Instance->ComputeShaderMap[Desc.ShaderName.c_str()] = new ComputeShader(Desc);
	}
}

void ShaderManager::AddRenderShader(const wstring& InName, RenderingShader* InShader)
{
	ASSERT(InShader != nullptr, "InShader Not Valid")
}

void ShaderManager::AddComputeShader(const wstring& InName, ComputeShader* InShader)
{
	ASSERT(InShader != nullptr, "InShader Not Valid");
}

ShaderManager::ShaderManager()
= default;

ShaderManager::~ShaderManager()
{
	for (auto & P : SamplerStateTable)
		SAFE_RELEASE(P.second);
	for (auto & P : DepthStencilStateTable)
		SAFE_RELEASE(P.second);
	for (auto & P : BlendStateTable)
		SAFE_RELEASE(P.second);
	for (auto & P : RasterizerStateTable)
		SAFE_RELEASE(P.second);
	for (auto & P : RenderShaderMap)
		SAFE_DELETE(P.second);
	for (auto & P : ComputeShaderMap)
		SAFE_DELETE(P.second);
}

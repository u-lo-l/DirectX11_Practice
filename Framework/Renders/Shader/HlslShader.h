#pragma once

#include <string>
#include <map>
using std::string;
using std::wstring;
using std::map;
#include <d3d11.h>

enum class ShaderType : UINT
{
	PixelShader		= 1 << 0,
	VertexShader	= 1 << 1,
	GeometryShader	= 1 << 2,
	HullShader		= 1 << 3,
	DomainShader	= 1 << 4,
};

template <class T>
class HlslShader
{
public:
	constexpr static int VertexSlot = 0;
	constexpr static int InstanceSlot = 9;
private:
	static string GetShaderTarget( ShaderType Type );
	// ! 우선 하나의 Shader에 하나의 entry_point만 있다고 가정하자.
	// ! 물론 여러 함수를 하나의 hlsl에 작성하고 선택 할 수도 있겠지.
	// ! 그건 나중에 처리하자.
	static string GetEntryPoint( ShaderType Type );
public:
	explicit HlslShader(const wstring & ShaderFileName, UINT TargetShaderFlag = ((UINT)ShaderType::VertexShader | (UINT)ShaderType::PixelShader));
	~HlslShader();
private:
	wstring FileName;

public:
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY InTopology);
	// Rasterizer
	HRESULT CreateRasterizerState_WireFrame();
	HRESULT CreateRasterizerState_Solid();
	HRESULT CreateRasterizerState_WireFrame_NoCull();
	HRESULT CreateRasterizerState_Solid_NoCull();
	HRESULT CreateRasterizerState_Solid_CW();
	HRESULT CreateRasterizerState(const D3D11_RASTERIZER_DESC * RSDesc);

	// Sampler
	HRESULT CreateSamplerState_Linear();
	HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC * SampDesc, SamplerSlot SlotNum = SamplerSlot::PS_Linear);

	// Blend
	HRESULT CreateBlendState_NoBlend();
	HRESULT CreateBlendState_AlphaBlend();
	HRESULT CreateBlendState_AlphaBlendCoverage();
	HRESULT CreateBlendState(const D3D11_BLEND_DESC * BlendDesc);

	//DepthStencil
	HRESULT CreateDepthStencilState_Default();
	HRESULT CreateDepthStencilState_NoDepth();
	HRESULT CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC * DepthStencilDesc);
private:
	SamplerSlot SamplerSlotNum = SamplerSlot::PS_Linear;
	// Draw
public:
	void Draw(UINT VertexCount, UINT StartVertexLocation = 0);
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation = 0, UINT BaseVertexLocation = 0);
	void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation = 0, UINT StartInstanceLocation = 0);
	void DrawIndexedInstanced(UINT IndexCountPreInstance, UINT InstanceCount, UINT StartIndexLocation = 0, INT BaseVertexLocation = 0, UINT StartInstanceLocation = 0);

private:
	void CompileShader(ShaderType Type, const wstring & ShaderFileName);
	void InitializeInputLayout(ID3DBlob * VertexShaderBlob);

	void BeginDraw();
	void EndDraw();
private:
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ID3D11InputLayout * InputLayout = nullptr;
	
	ID3D11RasterizerState * RasterizerState = nullptr;
	ID3D11SamplerState * SamplerState = nullptr;
	ID3D11BlendState * BlendState = nullptr;
	ID3D11DepthStencilState * DepthStencilState = nullptr;

	ID3D11RasterizerState * Prev_RasterizerState = nullptr;
	ID3D11SamplerState * Prev_SamplerState = nullptr;
	ID3D11BlendState * Prev_BlendState = nullptr;
	ID3D11DepthStencilState * Prev_DepthStencilState = nullptr;

	ID3D11VertexShader * VertexShader = nullptr;
	ID3D11PixelShader * PixelShader = nullptr;
	ID3D11GeometryShader * GeometryShader = nullptr;
};


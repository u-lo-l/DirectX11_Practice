#pragma once

template <class T,  enable_if_t<is_base_of_v<ShaderInputType, T>, int> = 0>
class HlslShader
{
public:
	constexpr static int VertexSlot = 0;
	constexpr static int InstanceSlot = 9;
private:
	static constexpr string GetShaderTarget(D3D11_SHADER_VERSION_TYPE Type);
	// ! 우선 하나의 Shader에 하나의 entry_point만 있다고 가정하자.
	// ! 물론 여러 함수를 하나의 hlsl에 작성하고 선택 할 수도 있겠지.
	// ! 그건 나중에 처리하자.
	static constexpr string GetEntryPoint(D3D11_SHADER_VERSION_TYPE Type);
public:
	HlslShader(const wstring & VsFileName, const wstring & PsFileName, const wstring & CsFileName = L"");
	~HlslShader();
public:
	void Draw(UINT VertexCount, UINT StartVertexLocation = 0) const;
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation = 0, UINT BaseVertexLocation = 0) const;
	void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation = 0, UINT StartInstanceLocation = 0) const;
	void DrawIndexedInstanced(UINT IndexCountPreInstance, UINT InstanceCount, UINT StartIndexLocation = 0, INT BaseVertexLocation = 0, UINT StartInstanceLocation = 0) const;
	void Dispatch(UINT X, UINT Y, UINT Z) const;
	
private:
	HlslShader(const HlslShader&) = delete;
	HlslShader& operator=(const HlslShader&) = delete;
	HlslShader(HlslShader&&) = delete;
	HlslShader& operator=(HlslShader&&) = delete;

private:
	void CompileShader(D3D11_SHADER_VERSION_TYPE Type, const wstring & ShaderFileName);
	void InitializeInputLayout(ID3DBlob * VertexShaderBlob);

	void BeginDraw() const;
	void EndDraw() const;
private:
	ID3D11DeviceContext * const DeviceContext;
	ID3D11VertexShader * VertexShader;
	ID3D11PixelShader * PixelShader;
	ID3D11ComputeShader * ComputeShader;
	ID3D11InputLayout * InputLayout;
};

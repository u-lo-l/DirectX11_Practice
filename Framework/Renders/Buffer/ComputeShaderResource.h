#pragma once

// Abstract
/*
 * ComputeShader에 필요한 자원을 관리하는 클래스
 * SRV를 통해 Shader에 Input을 넣어주고 UAV를 통해 Shader로부터 Output을 받는다.
 */
class ComputeShaderResource
{
public:
	ComputeShaderResource() = default;
	virtual ~ComputeShaderResource();
	ComputeShaderResource(const ComputeShaderResource &) = delete;
	ComputeShaderResource(ComputeShaderResource &&) = delete; 
	ComputeShaderResource & operator=(const ComputeShaderResource &) = delete;
	ComputeShaderResource & operator=(ComputeShaderResource &&) = delete;

public:
	operator ID3D11UnorderedAccessView * ();
	operator ID3D11UnorderedAccessView * () const;
	operator const ID3D11UnorderedAccessView * ();
	operator const ID3D11UnorderedAccessView * () const;

protected:
	virtual void CreateInput() = 0;
	virtual void CreateSRV() = 0;
	virtual void CreateOutput() = 0;
	virtual void CreateUAV() = 0;
	virtual void CreateResult() = 0;

	void CreateBuffer();
protected:
	ID3D11Resource * Input = nullptr; // GPU input
	ID3D11ShaderResourceView * SRV = nullptr;

	ID3D11Resource * Output = nullptr; // from GPU to CPU output
	ID3D11UnorderedAccessView * UAV = nullptr;
	
	ID3D11Resource * Result = nullptr;
};
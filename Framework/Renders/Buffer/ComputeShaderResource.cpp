#include "framework.h"
#include "ComputeShaderResource.h"

ComputeShaderResource::~ComputeShaderResource()
{
	SAFE_RELEASE(Input);
	SAFE_RELEASE(Output);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(Result);
}

ComputeShaderResource::operator ID3D11UnorderedAccessView*()
{
	return UAV;
}

ComputeShaderResource::operator ID3D11UnorderedAccessView*() const
{
	return UAV;
}

ComputeShaderResource::operator const ID3D11UnorderedAccessView*()
{
	return UAV;
}

ComputeShaderResource::operator const ID3D11UnorderedAccessView*() const
{
	return UAV;
}

void ComputeShaderResource::CreateBuffer()
{
	CreateInput();
	CreateSRV();
	CreateOutput();
	CreateUAV();
	CreateResult();
}
#include "framework.h"
#include "VertexLayout.h"
#include "HlslShader.h"

// ! Position은 Vector(float3)인데 Float4로 들어감.
// [VertexShader 의미체계] https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics

// float3 Position
void Vertex::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(2);

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].SemanticIndex = 0; // 같은 SemanticName이 있을 때. ex) 다중 Texture포함
	OutLayoutDescs[0].InputSlot = HlslShader<Vertex>::VertexSlot;
	OutLayoutDescs[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	OutLayoutDescs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	OutLayoutDescs[0].InstanceDataStepRate = 0;

	OutLayoutDescs[1].SemanticName = "INSTANCE";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[1].SemanticIndex = 0;
	OutLayoutDescs[1].InputSlot = HlslShader<Vertex>::InstanceSlot;
	OutLayoutDescs[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[1].InstanceDataStepRate = 1;
}

// float3 Position, float4 Color
void VertexColor::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(2);

	for (int i = 0; i < 2; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<Vertex>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "COLOR";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// OutLayoutDescs[2].SemanticName = "INSTANCE";
	// OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	// OutLayoutDescs[2].SemanticIndex = 0;
	// OutLayoutDescs[2].InputSlot = HlslShader<VertexColor>::InstanceSlot;
	// OutLayoutDescs[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	// OutLayoutDescs[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	// OutLayoutDescs[2].InstanceDataStepRate = 1;
}

// float3 Position, float2 UV
void VertexTexture::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(3);

	for (int i = 0; i < 3; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTexture>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "UV";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;

	OutLayoutDescs[2].SemanticName = "INSTANCE";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[2].SemanticIndex = 0;
	OutLayoutDescs[2].InputSlot = HlslShader<VertexTexture>::InstanceSlot;
	OutLayoutDescs[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[2].InstanceDataStepRate = 1;
}

// float3 position, float2 UV, float4 Color
void VertexTextureColor::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(4);

	for (int i = 0; i < 4; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTextureColor>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "UV";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "COLOR";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	OutLayoutDescs[3].SemanticName = "INSTANCE";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[3].SemanticIndex = 0;
	OutLayoutDescs[3].InputSlot = HlslShader<VertexTextureColor>::InstanceSlot;
	OutLayoutDescs[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[3].InstanceDataStepRate = 1;
}

// float3 Position, float3 Normal
void VertexNormal::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(3);

	for (int i = 0; i < 3; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexNormal>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "NORMAL";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;

	OutLayoutDescs[2].SemanticName = "INSTANCE";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[2].SemanticIndex = 0;
	OutLayoutDescs[2].InputSlot = HlslShader<VertexNormal>::InstanceSlot;
	OutLayoutDescs[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[2].InstanceDataStepRate = 1;
}

// float3 Position, float2 UV, float3 Normal
void VertexTextureNormal::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(4);

	for (int i = 0; i < 4; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTextureNormal>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}
	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "UV";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "NORMAL";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;

	OutLayoutDescs[3].SemanticName = "INSTANCE";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[3].SemanticIndex = 0;
	OutLayoutDescs[3].InputSlot = HlslShader<VertexTextureNormal>::InstanceSlot;
	OutLayoutDescs[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[3].InstanceDataStepRate = 1;
}

// float3 Position, float2 UV, float4 Color, float3 Normal, float3 Tangent, float4 Indices, float4 Weights;
void ModelVertex::CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs )
{
	OutLayoutDescs.resize(11);

	for (int i = 0; i < 11; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<ModelVertex>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}
	OutLayoutDescs[0].SemanticName = "POSITION";
	// R32G32B32_FLOAT으로 지정해도 Shader에서 float4로 받을 수 있다.
	// Shader에서는 빈 w값을 자동으로 1.0f로 채워준다. Y, Z의 경우는 0으로 채운다.
	// https://stackoverflow.com/questions/29728349/hlsl-sv-position-why-how-from-float3-to-float4
	// https://learn.microsoft.com/en-us/previous-versions/windows/drivers/display/supplying-default-values-for-texture-coordinates-in-vertex-declaration
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "UV";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "COLOR";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	
	OutLayoutDescs[3].SemanticName = "NORMAL";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;

	OutLayoutDescs[4].SemanticName = "TANGENT";
	OutLayoutDescs[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[5].SemanticName = "BLENDINDICES";
	OutLayoutDescs[5].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	
	OutLayoutDescs[6].SemanticName = "BLENDWEIGHTS";
	OutLayoutDescs[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	OutLayoutDescs[7].SemanticName = "INSTANCE";
    OutLayoutDescs[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // 행렬이 4개 컬럼으로 전달됨
    OutLayoutDescs[7].SemanticIndex = 0;
    OutLayoutDescs[7].InputSlot = InstancingSlot;
    OutLayoutDescs[7].AlignedByteOffset = 0;
    OutLayoutDescs[7].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    OutLayoutDescs[7].InstanceDataStepRate = 1;

    // 각 행렬의 나머지 3개 부분
    OutLayoutDescs[8].SemanticName = "INSTANCE";
    OutLayoutDescs[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // 행렬의 2번째, 3번째, 4번째 컬럼
    OutLayoutDescs[8].SemanticIndex = 1;
    OutLayoutDescs[8].InputSlot = InstancingSlot;
    OutLayoutDescs[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    OutLayoutDescs[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    OutLayoutDescs[8].InstanceDataStepRate = 1;

	OutLayoutDescs[9].SemanticName = "INSTANCE";
	OutLayoutDescs[9].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // 행렬의 2번째, 3번째, 4번째 컬럼
	OutLayoutDescs[9].SemanticIndex = 2;
	OutLayoutDescs[9].InputSlot = InstancingSlot;
	OutLayoutDescs[9].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[9].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[9].InstanceDataStepRate = 1;

	OutLayoutDescs[10].SemanticName = "INSTANCE";
	OutLayoutDescs[10].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // 행렬의 2번째, 3번째, 4번째 컬럼
	OutLayoutDescs[10].SemanticIndex = 3;
	OutLayoutDescs[10].InputSlot = InstancingSlot;
	OutLayoutDescs[10].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[10].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[10].InstanceDataStepRate = 1;
}

void VertexBillboard::CreatInputLayout(vector<D3D11_INPUT_ELEMENT_DESC>& OutLayoutDescs)
{
	OutLayoutDescs.resize(4);

	for (int i = 0; i < 4; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTextureColor>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "SCALE";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "MAPINDEX";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R8_UINT;

	OutLayoutDescs[3].SemanticName = "INSTANCE";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[3].SemanticIndex = 0;
	OutLayoutDescs[3].InputSlot = HlslShader<VertexBillboard>::InstanceSlot;
	OutLayoutDescs[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[3].InstanceDataStepRate = 1;
}

void VertexPrecipitation::CreatInputLayout(vector<D3D11_INPUT_ELEMENT_DESC>& OutLayoutDescs)
{
	OutLayoutDescs.resize(4);

	for (int i = 0; i < 4; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTextureColor>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "SCALE";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "RANDOM";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32_FLOAT;

	OutLayoutDescs[3].SemanticName = "INSTANCE";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[3].SemanticIndex = 0;
	OutLayoutDescs[3].InputSlot = HlslShader<VertexBillboard>::InstanceSlot;
	OutLayoutDescs[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[3].InstanceDataStepRate = 1;
}

void VertexParticle::CreatInputLayout(vector<D3D11_INPUT_ELEMENT_DESC>& OutLayoutDescs)
{
	OutLayoutDescs.resize(6);

	for (int i = 0; i < 6; i++)
	{
		OutLayoutDescs[i].SemanticIndex = 0;
		OutLayoutDescs[i].InputSlot = HlslShader<VertexTextureColor>::VertexSlot;
		OutLayoutDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		OutLayoutDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		OutLayoutDescs[i].InstanceDataStepRate = 0;
	}

	OutLayoutDescs[0].SemanticName = "POSITION";
	OutLayoutDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[1].SemanticName = "START_VELOCITY";
	OutLayoutDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[2].SemanticName = "END_VELOCITY";
	OutLayoutDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	
	OutLayoutDescs[3].SemanticName = "RANDOM";
	OutLayoutDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	OutLayoutDescs[4].SemanticName = "TIME";
	OutLayoutDescs[4].Format = DXGI_FORMAT_R32_FLOAT;
	
	OutLayoutDescs[5].SemanticName = "INSTANCE";
	OutLayoutDescs[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	OutLayoutDescs[5].SemanticIndex = 0;
	OutLayoutDescs[5].InputSlot = HlslShader<VertexBillboard>::InstanceSlot;
	OutLayoutDescs[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	OutLayoutDescs[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	OutLayoutDescs[5].InstanceDataStepRate = 1;
}

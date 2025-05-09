#include "Slot.SkyBox.hlsl"
cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF;
}

cbuffer CB_ViewProjection : register(Const_VS_ViewProjection)
{ 
    matrix View;
    matrix ViewInv;
    matrix Projection;
}
struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    // SkyBox(or SkySphere)의 Local Vertex Position
    float3 LocalPosition : Position1;
};

VertexOutput VSMain(VertexInput input)
{    
    VertexOutput output;
    
    output.LocalPosition = input.Position.xyz;
    
    output.Position = mul(input.Position, WorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

SamplerState LinearSampler : register(Sampler_PS_Linear);
TextureCube CubeMap : register(Texture_PS_CubeMap);

float4 PSMain(VertexOutput input) : SV_Target
{
    return CubeMap.Sample(LinearSampler, input.LocalPosition);
}
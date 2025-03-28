#include "../Slot.hlsl"
#include "../00_WorldViewProjection.hlsl"

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 oPosition : Position1;
};

VertexOutput VSMain(VertexInput input)
{    
    VertexOutput output;
    
    output.oPosition = input.Position.xyz;
    
    output.Position = mul(input.Position, WorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

SamplerState LinearSampler : register(Sampler_PS_Linear);
TextureCube CubeMap : register(t0);

float4 PSMain(VertexOutput input) : SV_Target
{
    return CubeMap.Sample(LinearSampler, input.oPosition);
}

// technique11 T0
// {
//     P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS, PS)
// }
#include "Slot.hlsl"

cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF;
}

cbuffer CB_LightDirection : register(Const_PS_LightDirection)
{ 
    float3 LightDirection;
}

cbuffer CB_ViewProjection : register(Const_GS_VieProjection)
{ 
    matrix GS_View;
    matrix GS_ViewInv;
    matrix GS_Projection;
}

struct VertexInput
{
    float4 Position : POSITION;
    float2 Scale    : SCALE;
    uint   MapIndex : MAPINDEX;
    
    uint VertexIndex : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float2 Scale    : SCALE;
    uint   MapIndex : MAPINDEX;
};

struct GeometryOutput
{
    float4 Position : SV_Position;
    float2 Uv       : UV;
    uint   MapIndex : MAPINDEX;
};

SamplerState LinearSampler : register(Sampler_PS_Linear);
Texture2DArray BillboardMap : register(t0);

/*==========================================================*/

VertexOutput VSMain(VertexInput input)
{    
    VertexOutput output;
    
    output.Position = mul(input.Position, WorldTF);
    output.Scale = input.Scale;
    output.MapIndex = input.MapIndex;
    
    return output;
}

[maxvertexcount(4)]
void GSMain(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = normalize(input[0].Position.xyz - GS_ViewInv._41_42_43);
    float3 right = normalize(cross(up, forward));
    
    float2 size = input[0].Scale * 0.5f;
    
    
    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1);
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1);
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1);
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1);
    
    float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    
    GeometryOutput output;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(position[i], GS_View);
        output.Position = mul(output.Position, GS_Projection);
        output.Uv = uv[i];
        output.MapIndex = input[0].MapIndex;
        
        stream.Append(output);
    }
}

// Rasterizer state : Cull_NONE (양면)
// BlendState : AlphaBlend (bs, float4(0, 0, 0, 0), 0xFF);
float4 PSMain(GeometryOutput input) : SV_Target
{
    const float ScaleFactor = 1.75f;
    return BillboardMap.Sample(LinearSampler, float3(input.Uv, input.MapIndex)) * ScaleFactor;
}
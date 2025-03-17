cbuffer WorldViewProjection : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

cbuffer LerpRateBuffer : register(b1)
{
    float LerpRate;
    float3 Padding;
};

Texture2D Map : register(t0);

SamplerState Sampler : register(s0);

struct VertexInput
{
    float4 Position : POSITION;
    float2 UV : Uv;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 UV : Uv;
    float4 Color : Color;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.UV = input.UV;
    output.Color = input.Color;

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    float4 Color1 = input.Color;
    float4 Color2 = Map.Sample(Sampler, input.UV);
    // float4 Color1 = float4(1,0,0,1);
    // float4 Color2 = float4(0,1,0,1);
    return lerp(Color1, Color2, LerpRate);
}

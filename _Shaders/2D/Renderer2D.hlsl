#define Slot_Const_World_VS b0
#define Slot_Const_ViewProjection_VS b1
#define Slot_Texture_Texture t0
#define Slot_Sampler_Linear s0

cbuffer CB_World : register(Slot_Const_World_VS)
{
    matrix World;
}

cbuffer CB_Render2D : register(Slot_Const_ViewProjection_VS)
{
    matrix View2D;
    matrix Projection2D;
};

Texture2D Texture : register (Slot_Texture_Texture);
SamplerState LinearSampler : register(Slot_Sampler_Linear);

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    output.Uv = input.Uv;
    
    return output;
}


float4 PSMain(VertexOutput input) : SV_Target
{
    // return (1,1,1,1);
    return Texture.Sample(LinearSampler, input.Uv);
}

cbuffer WorldViewProjection : register(b0)
{
    matrix WorldMat;
    matrix ViewMat;
    matrix ProjectionMat;
};

struct VertexInput
{
    float4 Position : POSITION;
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : Normal;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, WorldMat);
    output.Position = mul(output.Position, ViewMat);
    output.Position = mul(output.Position, ProjectionMat);
    
    output.Normal = mul(input.Normal, (float3x3)WorldMat);
    
    return output;
}

float3 LightDirection = float3(-1,-1,1);
float4 PSMain(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 color = dot(-normalize(LightDirection), normal);
    return float4(color, 1.0f);
}
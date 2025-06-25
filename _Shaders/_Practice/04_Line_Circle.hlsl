struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float4 Color : Color;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Color = input.Color;

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return input.Color; // r,g,b,a
}
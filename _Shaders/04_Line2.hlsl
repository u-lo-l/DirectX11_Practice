struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : Color;
    
    uint VertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float4 Color : Color;
    uint VertexID : VertexID;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Color = input.Color;
    output.VertexID = input.VertexID;

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    if (input.VertexID == 2 || input.VertexID == 3)
        return float4(1, 1, 1, 1);
    
    return input.Color; // r,g,b,a
}
#ifndef __FOLIAGE_HLSL__
# define __FOLIAGE_HLSL__

float Rand(float2 p)
{
    // 임의의 큰 상수와 sin/frac을 조합
    float h = dot(p, float2(127.1, 311.7));
    return frac((sin(h) + 1.f) * 0.5f);
}

cbuffer CB_WVP : register(b0)  // VS GS
{
    matrix World      : packoffset(c0);
    matrix View       : packoffset(c4);
    matrix Projection : packoffset(c8);
};

cbuffer CB_Terrain : register(b1) // VS PS
{
    float3  LightDirection;
    float   HeightScaler;
    float2  TerrainMapSize;
    float2  TexelSize;
};

cbuffer CB_DensityDistance : register(b2) // GS
{
    float3 CameraPosition;
    float Near;
    float Far;
    float Padding[3];
};

SamplerState LinearSampler : register(s0);      // VS DS PS
SamplerState AnisotropicSampler : register(s1); // VS DS PS

Texture2D      TerrainHeightMap : register(t0); // VS PS
Texture2DArray FoliageTextures : register(t1);  // PS

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 Scale    : SCALE;
    uint   MapIndex : MAPINDEX;
};

struct VS_OUTPUT    // GS_INPUT
{
    float4 Position : POSITION;
    float2 Scale    : SCALE;
    float2 UV       : UV;
    uint   MapIndex : MAPINDEX;
};
struct GS_OUTPUT    // PS_INPUT
{
    float4 Position : SV_Position;
    float2 UV       : UV;
    float2 TexCord  : TEXCORD;
    uint   MapIndex : MAPINDEX;
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT output;
    
    output.UV = float2(Input.Position.x / TerrainMapSize.x, Input.Position.z / TerrainMapSize.y);
    float Height = TerrainHeightMap.SampleLevel(AnisotropicSampler, output.UV, 0).r;
    Input.Position.y = Height * HeightScaler;

    output.Position = mul(Input.Position, World);
    output.Scale = Input.Scale;
    output.MapIndex = Input.MapIndex;
    
    return output;
}

void Foliage_Billboard(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream);
void Foliage_CrossQuad(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream);
void Foliage_Triangle(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream);

[maxvertexcount(12)]
void GSMain(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream)
{
    float3 ViewSpacePosition = mul(input[0].Position, View).xyz;
    [flatten] if (ViewSpacePosition.z <= 0)
    {
        return ;
    }

    float DistanceFromCamera = length(ViewSpacePosition);
    float random_val = Rand(input[0].Position.xz);
    float RenderProbability = 0.0f;

    RenderProbability = smoothstep(.5f, 1.f, (DistanceFromCamera) / (Near));
    if (DistanceFromCamera <= Near && random_val <= RenderProbability)
    {
        Foliage_Triangle(input[0], stream);
        return ;
    }
    RenderProbability = smoothstep(.3f, 0.6f, (Near - DistanceFromCamera) / (Far - Near));
    if (DistanceFromCamera <= Far && random_val <= RenderProbability)
    {
        float t = saturate((Far - DistanceFromCamera) / (Far - Near)); // Cross 확률
        if (random_val <= t)
            Foliage_CrossQuad(input[0], stream);
        else
            Foliage_Billboard(input[0], stream);
        return ;
    }
    RenderProbability = smoothstep(0.f, 0.4f, (Far - DistanceFromCamera) / (1000.f - Far));
    if (DistanceFromCamera <= 1000.0f && random_val <= RenderProbability)
    {
        float t = saturate((1000.0f - DistanceFromCamera) / (1000.0f - Far)); // Billboard 확률
        if (random_val <= t)
            Foliage_Billboard(input[0], stream);
        return ;
    }

    // float RenderProbability = 0.0f;

    // RenderProbability = smoothstep(.5f, 1.f, (DistanceFromCamera) / (Near));
    // if (DistanceFromCamera <= Near && random_val <= RenderProbability)
    // {
    //     Foliage_Triangle(input[0], stream);
    //     return;
    // }
    // RenderProbability = smoothstep(.3f, 0.6f, (Near - DistanceFromCamera) / (Far - Near));
    // if(DistanceFromCamera <= Far && random_val <= RenderProbability)
    // {
    //     Foliage_CrossQuad(input[0], stream);
    //     return ;
    // }
    // RenderProbability = smoothstep(0.f, 0.4f, (Far - DistanceFromCamera) / (1000.f - Far));
    // if (DistanceFromCamera <= 1000.f && random_val <= RenderProbability)
    // {
    //     Foliage_Billboard(input[0], stream);
    //     return ;
    // }
}

// RasterizerState : Cull_NONE
// BlendState : AlphaBlend_Coverage
float3 CalculateNormal(float2 UV);
float4 PSMain(GS_OUTPUT input) : SV_Target
{
    const float ScaleFactor = 1.75f;
    Folia
    float4 Color = FoliageTextures.Sample(LinearSampler, float3(input.TexCord, input.MapIndex));
    float LDotN = dot(-normalize(LightDirection), CalculateNormal(input.UV));
    return float4(Color.rgb * LDotN * ScaleFactor, Color.a);
}


/*==============================================================================================*/

float3 CalculateNormal(float2 UV)
{
    float2 dUV[4] = {
        float2(-TexelSize.x, 0), 
        float2(+TexelSize.x, 0),
        float2(0, -TexelSize.y),
        float2(0, +TexelSize.y)
    };
    float height[4] = {0, 0, 0, 0};

    [unroll(4)]
    for(int i = 0 ; i < 4 ; i++)
    {
        height[i] = TerrainHeightMap.SampleLevel(AnisotropicSampler, UV + dUV[i], 0).r;
    }

    float StrideX = (UV.x - TexelSize.x < 0) || (UV.x + TexelSize.x > 1) ? 1.f : 2.f;
    float StrideZ = (UV.y - TexelSize.y < 0) || (UV.y + TexelSize.y > 1) ? 1.f : 2.f;
    float HeightDiffX = (height[1] - height[0]) * HeightScaler;
    float HeightDiffZ = (height[3] - height[2]) * HeightScaler;

    float3 tangent = normalize(float3(StrideX, HeightDiffX, 0));
    float3 bitangent = normalize(float3(0, HeightDiffZ, StrideZ));
    return normalize(cross(bitangent, tangent));
}

void Foliage_Billboard(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = normalize(Input.Position.xyz - CameraPosition);
    float3 right = normalize(cross(up, forward));

    Input.Position.y += Input.Scale.y * 0.3f;
    float2 size = Input.Scale * 0.5f;
    
    float4 position[4];
    position[0] = float4(Input.Position.xyz - size.x * right - size.y * up, 1);
    position[1] = float4(Input.Position.xyz - size.x * right + size.y * up, 1);
    position[2] = float4(Input.Position.xyz + size.x * right - size.y * up, 1);
    position[3] = float4(Input.Position.xyz + size.x * right + size.y * up, 1);
    
    float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };
    
    GS_OUTPUT output;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(position[i], mul(View, Projection));
        output.UV = Input.UV;
        output.TexCord = uv[i];
        output.MapIndex = Input.MapIndex;
        stream.Append(output);
    }
}

void Foliage_CrossQuad(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream)
{
    Input.Position.y += Input.Scale.y * 0.3f;
    float2 size = Input.Scale * 0.5f;
    float c45 = size.x * cos(radians(45));
    float s45 = size.x * sin(radians(45));
    
    float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    float4 position[2][4];
    position[0][0] = Input.Position + float4(-c45, -size.y, -s45, 0);
    position[0][1] = Input.Position + float4(-c45, +size.y, -s45, 0);
    position[0][2] = Input.Position + float4( c45, -size.y, +s45, 0);
    position[0][3] = Input.Position + float4( c45, +size.y, +s45, 0);

    position[1][0] = Input.Position + float4(+c45, -size.y, -s45, 0);
    position[1][1] = Input.Position + float4(+c45, +size.y, -s45, 0);
    position[1][2] = Input.Position + float4(-c45, -size.y, +s45, 0);
    position[1][3] = Input.Position + float4(-c45, +size.y, +s45, 0);

    GS_OUTPUT output;
    
    [unroll]
    for( int i = 0 ; i < 2 ; i++)
    {
        [unroll]
        for(int j = 0 ; j < 4 ; j++)
        {
            output.Position = mul(position[i][j], mul(View, Projection));
            output.UV = Input.UV;
            output.TexCord = uv[j];
            output.MapIndex = Input.MapIndex;
            stream.Append(output);
        }
        stream.RestartStrip();
    }
}

void Foliage_Triangle(VS_OUTPUT Input, inout TriangleStream<GS_OUTPUT> stream)
{
    Input.Position.y += Input.Scale.y * 0.3f;
    float2 size = Input.Scale * 0.5f;
    float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    float c60 = size.x * cos(radians(60));
    float s60 = size.x * sin(radians(60));
    float c30 = size.x * cos(radians(30));
    float s30 = size.x * sin(radians(30));

    float4 position[3][4];
    float d = 1.f / 5.f;
    position[0][0] = Input.Position + float4(-size.x, -size.y, d * size.x, 0);
    position[0][1] = Input.Position + float4(-size.x, +size.y, d * size.x, 0);
    position[0][2] = Input.Position + float4(+size.x, -size.y, d * size.x, 0);
    position[0][3] = Input.Position + float4(+size.x, +size.y, d * size.x, 0);
    
    position[1][0] = Input.Position + float4(-c60, -size.y, -s60, 0) + d * float4(c30, 0, -s30, 0);
    position[1][1] = Input.Position + float4(-c60, +size.y, -s60, 0) + d * float4(c30, 0, -s30, 0);
    position[1][2] = Input.Position + float4( c60, -size.y, +s60, 0) + d * float4(c30, 0, -s30, 0);
    position[1][3] = Input.Position + float4( c60, +size.y, +s60, 0) + d * float4(c30, 0, -s30, 0);

    position[2][0] = Input.Position + float4(+c60, -size.y, -s60, 0) + d * float4(-c30, 0, -s30, 0);
    position[2][1] = Input.Position + float4(+c60, +size.y, -s60, 0) + d * float4(-c30, 0, -s30, 0);
    position[2][2] = Input.Position + float4(-c60, -size.y, +s60, 0) + d * float4(-c30, 0, -s30, 0);
    position[2][3] = Input.Position + float4(-c60, +size.y, +s60, 0) + d * float4(-c30, 0, -s30, 0);

    GS_OUTPUT output;
    
    [unroll]
    for( int i = 0 ; i < 3 ; i++)
    {
        [unroll]
        for(int j = 0 ; j < 4 ; j++)
        {
            output.Position = mul(position[i][j], mul(View, Projection));
            output.UV = Input.UV;
            output.TexCord = uv[j];
            output.MapIndex = Input.MapIndex;
            stream.Append(output);
        }
        stream.RestartStrip();
    }
}


#endif
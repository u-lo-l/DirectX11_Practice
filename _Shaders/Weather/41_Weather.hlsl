#include "Slot.Weather.hlsl"

cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF;
}

cbuffer CB_ViewProjection_GS : register(Const_GS_VieProjection)
{ 
    matrix GS_View;
    matrix GS_ViewInv;
    matrix GS_Projection;
}

cbuffer CB_WeatherData_VS : register(Const_VS_WeatherData)
{
	float3 Velocity;
	float  Padding;
	
	float3 Origin;
	float  Turbulence;
	
	float3 Extent;
    float  Time;
}

cbuffer CB_WeatherData_GS : register(Const_GS_WeatherData)
{
    float3 Up;
    float DrawDistance;
}

cbuffer CB_WeatherData_PS : register(Const_PS_WeatherData)
{
    float4 Color;
}

SamplerState LinearSampler : register(Sampler_PS_Linear);
Texture2D DiffuseMap : register(Texture_PS_Diffuse);

struct VS_Input
{
	float4 Position : POSITION;
	float2 Scale    : SCALE;
	float2 Random   : RANDOM;
};

struct VS_Output
{
    float4 Position : POSITION;
    float2 Scale    : SCALE;
};

struct GS_Output
{
    float4 Position : SV_Position;
    float2 Uv       : UV;
    float  Alpha    : ALPHA;
};

VS_Output VSMain(VS_Input input)
{
    VS_Output output;
    
    float3 displace = Velocity * Time;
    
    input.Position.y = Origin.y + Extent.y - (input.Position.y - displace.y) % Extent.y;
    input.Position.x += cos(Time - input.Random.x) * Turbulence;
    input.Position.z += cos(Time - input.Random.y) * Turbulence;
    
    input.Position.xyz = Origin + (Extent + (input.Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f);
        
    output.Position = mul(input.Position, WorldTF);
    output.Scale = input.Scale;
    
    return output;
}

[maxvertexcount(4)]
void GSMain(point VS_Output input[1], inout TriangleStream<GS_Output> stream)
{
    float3 up = normalize(-Up);
    float3 CameraPosition = GS_ViewInv._41_42_43;
    float3 forward = normalize(input[0].Position.xyz - CameraPosition);
    float3 right = normalize(cross(up, forward));
    
    float2 size = input[0].Scale * 0.5f;
    
    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1);
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1);
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1);
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1);
    
    float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    GS_Output output;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        float4 view = 0;
        view = mul(position[i], GS_View);
        output.Position = mul(view, GS_Projection);
        output.Uv = uv[i];
        output.Alpha = saturate(1 - view.z / DrawDistance) * 0.5f;
        stream.Append(output);
    }
}

float4 PSMain(GS_Output input) : SV_Target
{
    float4 TexColor = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    TexColor.rgb = Color.rgb;
    TexColor.a = TexColor.a * input.Alpha * 1.5f;
    
    return TexColor;
}

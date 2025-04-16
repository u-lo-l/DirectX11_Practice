#include "42_ParticleSystem.Functions.hlsl"

struct VS_Input
{
	float4 Position      : POSITION;
	float3 StartVelocity : START_VELOCITY;
	float3 EndVelocity 	 : END_VELOCITY;
	float4 Random        : RANDOM;
	float  Time          : TIME;
};

struct VS_Output
{
	float4 Position : POSITION;
	float4 Random   : RANDOM;
	float  Time     : TIME;
};

struct GS_Output
{
	float4 Position : SV_Position;
	float2 Uv       : GS_UV;
	float4 Color    : GS_COLOR;
};

VS_Output VSMain(VS_Input input)
{    
	VS_Output output;
    
	float age = 0.0f;
	float normalizedAge = 0.0f;
    
	VS_CalculateAge(input.Time, input.Random.x, age, normalizedAge);
	output.Position = VS_CalculatePosition(
		input.Position.xyz,
		input.StartVelocity,
		input.EndVelocity,
		age, 
		normalizedAge
	);
	output.Random = input.Random;
	output.Time = input.Time;

	return output;
}

[maxvertexcount(4)]
void GSMain(point VS_Output input[1], inout TriangleStream<GS_Output> stream)
{
	float age = 0.0f;
	float normalizedAge = 0.0f;

	GS_CalculateAge(input[0].Time, input[0].Random.x, age, normalizedAge);

	float2 size = GS_CalculateSize(input[0].Random.y, normalizedAge);
	float2x2 rotation = GS_CalculateRotation(input[0].Random.z, age);
	float4 color = GS_CalculateColor(input[0].Random.w, normalizedAge);
	float2 PatchCorner[4];
	PatchCorner[0] = float2(-1.0f, -1.0f);
	PatchCorner[1] = float2(-1.0f, +1.0f);
	PatchCorner[2] = float2(+1.0f, -1.0f);
	PatchCorner[3] = float2(+1.0f, +1.0f);

	GS_Output output;
	// PatchCorner값을 사용하여 한 변의 길이가 size인 정사각형을 만든다.
	// W-divide 이전의 데이터이므로 PS에서 W-divide가 수행되면 한 변의 길이가 (size/W)인 정사각형이 된다.
	// 이 떄 W값은 View공간에서의 Z값으로, Eye(Camera)로부터 멀 수록 작은 Patch가 만들어진다.
	[unroll(4)]
	for (int i = 0; i < 4; i++)
	{
		output.Position = input[0].Position;
		output.Position.xy += mul(PatchCorner[i], rotation) * size * 0.5f;
		output.Uv = (PatchCorner[i] + float2(1.0f, 1.0f)) * 0.5f;
		output.Color = color;
		stream.Append(output);
	}
}

float4 PSMain(GS_Output input) : SV_Target
{
	float4 result = ParticleMap.Sample(LinearSampler, input.Uv) * input.Color;
	return result * 1.75f;
}


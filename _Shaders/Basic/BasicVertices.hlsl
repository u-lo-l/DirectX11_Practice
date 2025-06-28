#ifndef __BASIC_VERTICES_HLSL__
#define __BASIC_VERTICES_HLSL__

cbuffer CB_Matrix : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_Input
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VSMain(VS_Input Input)
{
	VS_OUTPUT Output;
	Output.Position = Input.Position;
	Output.Position = mul(Input.Position, World);
	Output.Position = mul(Input.Position, View);
	Output.Position = mul(Input.Position, Projection);
	Output.Color = Input.Color;
	return Output;
}

float4 PSMain(VS_OUTPUT Input) : SV_TARGET
{
	return float4(Input.Color.rgb, 1.f);
}

#endif
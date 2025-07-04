#ifndef __SKYBOX_HLSL__
#define __SKYBOX_HLSL__

#include "../PerFrame.hlsli"

SamplerState		LinearSampler_Wrap : register(s0);
TextureCube<float4> CubeMap : register(t10);

cbuffer CB_World : register(b1)
{
	matrix WorldTF;
}

struct VertexInput
{
	float4 Position : Position;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float3 LocalPosition : Position1;
};

VertexOutput VSMain(VertexInput input)
{
	VertexOutput output;

	output.LocalPosition = input.Position.xyz;

	// output.Position = input.Position + float4(CameraWorldPosition, 1);
	output.Position = mul(input.Position, WorldTF);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
	return CubeMap.Sample(LinearSampler_Wrap, input.LocalPosition);
}
#endif
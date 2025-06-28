# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
# ifndef MAX_TESS_FACTOR
#  define MAX_TESS_FACTOR 64
# endif

#include "../PerFrame.hlsli"

struct VS_INPUT
{
	// Vertex
	float4 Position : POSITION;
	float2 UV : UV;
	uint ID : SV_VERTEXID;
};

struct VS_OUTPUT // HS_INPUT
{
	float4 Position : POSITION;
};

struct HS_CONSTANT_OUTPUT
{
	float Edge[4] : SV_TessFactor;
	float Inside[2] : SV_InsideTessFactor;
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float4 WorldPosition : WPOSITION;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position	: SV_Position;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	return output;
}

// HS
HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;
	output.Inside[0] = 4;
	output.Inside[1] = 4;
	output.Edge[0] = 4;
	output.Edge[1] = 4;
	output.Edge[2] = 4;
	output.Edge[3] = 4;
	return output;
}

[domain(DOMAIN)]
[partitioning(HS_PARTITION)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(HS_OUTPUT_PATCH_SIZE)]
[patchconstantfunc("HSConstant")]
HS_POINT_OUTPUT HSMain // Pass Through
(
	InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
	uint id : SV_OutputControlPointID
)
{
	HS_POINT_OUTPUT output;
	output.WorldPosition = patch[id].Position;
	return output;
}

[domain(DOMAIN)]
DS_OUTPUT DSMain
(
    HS_CONSTANT_OUTPUT input,
    float2 UV : SV_DomainLocation,
    const OutputPatch<HS_POINT_OUTPUT, HS_OUTPUT_PATCH_SIZE> patch
)
{
	DS_OUTPUT output;

	float4 v1 = lerp(patch[0].WorldPosition, patch[1].WorldPosition, UV.x);
	float4 v2 = lerp(patch[3].WorldPosition, patch[2].WorldPosition, UV.x);
	output.Position = lerp(v1, v2, UV.y);

	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	return output;
}

float4 PSMain(DS_OUTPUT Input) : SV_TARGET
{
	return float4(1,1,1,1);
}
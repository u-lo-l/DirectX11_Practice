cbuffer CB_World
{
	matrix World;
}
/**
 * Constant Buffer
 */
cbuffer CB_Context
{
	matrix View;
	matrix ViewInv;
	matrix Projection;
	matrix ViewProjection;

	float3 LightDirection;
}

cbuffer CB_Frame
{
    uint frame;
    float3 padding;
};
cbuffer CB_World
{
	matrix ModelWorldTF;
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
#ifndef __OCEAN_SHADING_HLSL__
#define __OCEAN_SHADING_HLSL__

/**************************************************************************************/
/*                                                                                    */
/* [Reference]                                                                        */
/* University of Leeds - Saulius Vincevicius, Final Report                            */
/* COMP3931 Individual Project: Realistic Ocean Simulation using Fourier Transform    */
/* Some concepts and implementation details were inspired by this project.            */
/* https://github.com/Biebras/Ocean-Simulation-Unity                                  */
/*                                                                                    */
/**************************************************************************************/

float3 commutator(float3 A, float3 B);
/*
 * SYMBOL
 * L = L_a + L_ss + L_s + L_r
 * L_a  : Ambient
 * L_ss : Surface Scattering
 * L_s  : Specular
 * L_r  : Environment Reflection
 *
 * N     : Surface Normal
 * D_s   : Sun Direction ( Light Direction )
 * D_v   : View Direction ( Camera Forward )
 * D_i   : Camera To Fragment Direction ( CameraSpace Position )
 *
 * C_a   : Ambient Color
 * C_l   : Light Color
 * C_b   : Air Bubble Color
 * C_s   : Specular Color
 * C_ws  : Water Scattering Color
 * C_sky : Sky Color
 *
 * H     : Ocean Height
 * F     : Fresnel Effect
 * rho   : AirBubble Density
 *
 * k_a   : Ambient Intensity
 * k_ss1 : Surface Scattering Intensity1
 * k_ss2 : Surface Scattering Intensity2
 * k_R   : Reflection Intensity
 *
*/

float CrossMag(float3 A, float3 B)
{
	return length(cross(A, B));
}

float3 OceanShading
(
	out float3 L_a,
	out float3 L_ss,
	out float3 L_s,
	out float3 L_r,
	out float F,
	float3 N,     // Surface Normal
	float3 D_s,   // Sun Direction ( -Light Direction )
	float3 D_v,   // View Direction (WorldCameraPos - WorldPosition)
	float3 D_r,

	float3 C_l,   // Light Color
	float3 C_b,   // Air Bubble Color
	float3 C_s,   // Specular Color
	float3 C_ws,  // Water Scattering Color
	float3 C_sky, // Sky Color

	float H,     // Ocean Height
	float rho,   // AirBubble Density

	float k_a,   // Ambient Intensity
	float k_ss1, // Surface Scattering Intensity1
	float k_ss2, // Surface Scattering Intensity2
	float k_r    // Reflection Intensity
)
{
	const float WaterR0 = 0.02f;              // 수직 입사 반사 계수
	F = pow(1 - max(dot(D_v, N), 0.15f), 5); // Schlick's Approximation of Fresnel + 0.15보정
	const float3 L_ss1 = k_ss1 * max(0, H) * pow(CrossMag(D_s, -D_v), 4) * pow(0.5f - 0.5f * dot(D_s, N), 3);
	const float3 L_ss2 = k_ss2 * pow(CrossMag(D_v, N), 2);

	// const float spec = GetSpecularCoef(dot(D_v, N));
	const float Shininess = 10.f;
	float spec = pow(saturate(dot(D_v, D_r)), Shininess) * F;

	// GDC Conference by Mark Mihelich and Tim Tchebokov
	L_a = k_a * dot(N, D_s) * C_ws * C_l + rho * C_b * C_l;
	L_ss = (1 - F) * (L_ss1 + L_ss2) * C_ws * C_l;
	L_r = (WaterR0 + (1 - WaterR0) * F) * k_r * C_sky;
	L_s = C_s * spec * C_l;

	return L_a + L_ss + L_r + L_s;
}




#endif
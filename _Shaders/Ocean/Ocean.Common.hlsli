#ifndef __OCEAN_COMMON_HLSLI__
#define __OCEAN_COMMON_HLSLI__

float GetHorizontalScaler(float Tiling)
{
	return 1 / Tiling * 4;
}

float GetVerticalScaler(float HeightScalser, float Tiling)
{
	return HeightScalser * GetHorizontalScaler(Tiling);
}


#endif
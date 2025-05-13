#pragma once

class Texture;

class Noise
{
public:
	static Texture * CreateGaussian2DNoise(UINT InSize);
	static Texture * CreatePerlin2DNoise(UINT InSize);
};

#pragma once

class TextureArray;
class Texture;

class Noise
{
public:
	static Texture * CreateGaussian2DNoise(UINT InSize);
	static TextureArray * CreateGaussian2DNoiseArray(UINT InSize, UINT Count);
	static Texture * CreatePerlin2DNoise(UINT InSize);
};

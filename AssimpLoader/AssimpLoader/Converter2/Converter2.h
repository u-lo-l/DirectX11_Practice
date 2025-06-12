#pragma once

class Converter2
{
protected:
	Converter2();
	virtual ~Converter2() = 0;
	virtual void ReadAiScene(const wstring & InFileName);
	Assimp::Importer * Importer;

	const static unsigned int ConvertFlag;
	const static float PRE_Y_ROTATION;
};

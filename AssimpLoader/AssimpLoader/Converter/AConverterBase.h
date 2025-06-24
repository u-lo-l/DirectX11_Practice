#pragma once

class AConverterBase
{
protected:
	AConverterBase();
	virtual ~AConverterBase() = 0;
	virtual void ReadAiScene(const wstring & InFileName) = 0;
	Assimp::Importer * Importer;

	const static unsigned int ConvertFlag;
	const static float PRE_Y_ROTATION;
};

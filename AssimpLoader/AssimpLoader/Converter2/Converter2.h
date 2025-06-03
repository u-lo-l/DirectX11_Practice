#pragma once

class Converter2
{
protected:
	Converter2();
	virtual ~Converter2() = 0;
	virtual void ReadAiScene(const wstring & InFileName);
	// wstring FileName; //
	// wstring FilePath; // 전체 경로
	Assimp::Importer * Importer;

	const static unsigned int ConvertFlag;
	const static float PreYRotation;
};

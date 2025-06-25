#pragma once

class AConverterBase
{
protected:
	AConverterBase();
	virtual ~AConverterBase() = 0;
	virtual void ReadAiScene(const wstring & InFileName) = 0;
	Assimp::Importer * Importer;

	const static unsigned int ConvertFlag;
	constexpr static float PRE_Y_ROTATION = 3.14159274f;

	void ParseMetaData(const wstring& InFileName, const aiScene* InScene);
	void CreateConvertMatrix(int Forward, int Up, const wstring& InFileName);
	map<string, bool> MetaData_Bool; // AI_BOOL
	map<string, int> MetaData_Int;	// AI_INT32 AI_INT64 AI_UINT32 AI_UINT64
	map<string, float> MetaData_Float; // AI_FLOAT AI_DOUBLE
	map<string, Vector> MetaData_Vector; // AI_AIVECTOR3D
	map<string, string> MetaData_String; // AI_AISTRING

	Matrix CoordinateConvertMatrix;
};

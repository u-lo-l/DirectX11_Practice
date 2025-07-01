#pragma once

class Helper
{
public:
	static string ColorToJson(const class Color & InColor);
	static Color JsonToColor(const string& InJsonString);
	static Vector JsonToVector3(const string& InJsonString);
	static Quaternion JsonToQuaternion(const string& InJsonString);
	static Vector ConvertRhZUpNormalToLhYUp(const Vector & InNormal);
	static void SaveTextureAsFile(ID3D11Texture2D* InTexture, const wstring& FileName, UINT Slice = 0);

};
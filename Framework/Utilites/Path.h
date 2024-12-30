#pragma once

class Path
{
public:
	static bool bIsFileExist(const string & InPath);
	static bool bIsFileExist(const wstring & InPath);

	static bool bIsDirectoryExist(const string & InPath);
	static bool bIsDirectoryExist(const wstring & InPath);

	static string Combine(const string & InPath1, const string & InPath2);
	static wstring Combine(const wstring & InPath1, const wstring & InPath2);

	static string Combine(const vector<string> & InPaths);
	static wstring Combine(const vector<wstring> & InPaths);

	static string GetDirectoryName(string  InPath);
	static wstring GetDirectoryName(wstring  InPath);

	static string GetExtension(string  InPath);
	static wstring GetExtension(wstring  InPath);

	static string GetFileName(string InPath);
	static wstring GetFileName(wstring InPath);

	static string GetFileNameWithoutExtension(string InPath);
	static wstring GetFileNameWithoutExtension(wstring InPath);

	static bool bIsRelativePath(const string & InPath);
	static bool bIsRelativePath(const wstring & InPath);
};
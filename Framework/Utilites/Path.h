#pragma once

class Path
{
public:
	static void CreateFolder(const string& InFolderName);
	static void CreateFolder(const wstring& InFolderName);

	static void CreateFolders(string InFoldersName);
	static void CreateFolders(wstring InFoldersName);
	
	static bool IsFileExist(const string & InPath);
	static bool IsFileExist(const wstring & InPath);

	static bool IsDirectoryExist(const string & InPath);
	static bool IsDirectoryExist(const wstring & InPath);

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

	static bool IsRelativePath(const string & InPath);
	static bool IsRelativePath(const wstring & InPath);
};
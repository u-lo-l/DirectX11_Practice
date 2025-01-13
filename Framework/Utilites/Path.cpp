#include "Framework.h"
#include <shlwapi.h>
#include "Path.h"
#include "String.h"


#pragma comment(lib, "shlwapi.lib")

void Path::CreateFolder( const string & InFolderName )
{
	const bool bDirectoryExists = IsDirectoryExist(InFolderName);
	if (bDirectoryExists == false)
		CreateDirectoryA(InFolderName.c_str(), nullptr);
}

void Path::CreateFolder( const wstring & InFolderName )
{
	const bool bDirectoryExists = IsDirectoryExist(InFolderName);
	if (bDirectoryExists == false)
		CreateDirectoryW(InFolderName.c_str(), nullptr);
}

void Path::CreateFolders( string InFolderName )
{
	String::Replace(&InFolderName, "\\", "/");

	vector<string> folders;
	String::SplitString(&folders, InFolderName, "/");

	string temp = "";
	for (const string & folder : folders)
	{
		temp += folder + "/";

		CreateFolder(temp);
	}
}

void Path::CreateFolders( wstring InFolderName )
{
	String::Replace(&InFolderName, L"\\", L"/");

	vector<wstring> folders;
	String::SplitString(&folders, InFolderName, L"/");

	wstring temp = L"";
	for (const wstring & folder : folders)
	{
		temp += folder + L"/";

		CreateFolder(temp);
	}
}

bool Path::IsFileExist( const string & InPath)
{
	return IsFileExist(String::ToWString(InPath));
}

bool Path::IsFileExist(const wstring & InPath)
{
	DWORD fileValue = GetFileAttributes(InPath.c_str());

	return fileValue < 0xFFFFFFFF;
}

bool Path::IsDirectoryExist(const string & InPath)
{
	return IsDirectoryExist(String::ToWString(InPath));
}

bool Path::IsDirectoryExist(const wstring & InPath)
{
	DWORD attribute = GetFileAttributes(InPath.c_str());

	BOOL temp = (attribute != INVALID_FILE_ATTRIBUTES &&
		(attribute & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

string Path::Combine(const string & InPath1, const string & InPath2)
{
	return InPath1 + InPath2;
}

wstring Path::Combine(const wstring & InPath1, const wstring & InPath2)
{
	return InPath1 + InPath2;
}

string Path::Combine(const vector<string> & InPaths)
{
	string temp = "";
	for (const string & name : InPaths)
		temp += name;

	return temp;
}

wstring Path::Combine(const vector<wstring> & InPaths)
{
	wstring temp = L"";
	for (const wstring & name : InPaths)
		temp += name;

	return temp;
}

string Path::GetDirectoryName(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

wstring Path::GetDirectoryName(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

string Path::GetExtension(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

wstring Path::GetExtension(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

string Path::GetFileName(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

wstring Path::GetFileName(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

string Path::GetFileNameWithoutExtension(string InPath)
{
	string fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

wstring Path::GetFileNameWithoutExtension(wstring InPath)
{
	wstring fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

bool Path::IsRelativePath(const string & InPath)
{
	return IsRelativePath(String::ToWString(InPath));
}

bool Path::IsRelativePath(const wstring & InPath)
{
	BOOL b = PathIsRelative(InPath.c_str());

	return b >= TRUE;
}

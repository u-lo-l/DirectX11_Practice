#pragma once

class String
{
public:
	static void SplitString(vector<string>* InResult, const string & InOrigin, const string & InToken);
	static void SplitString(vector<wstring>* InResult, const wstring & InOrigin, const wstring & InToken);

	static bool StartsWith(const string & InValue, const string & InCompare);
	static bool StartsWith(const wstring & InValue, const wstring & InCompare);

	static bool Contain(const string & InValue, const string & InCompare);
	static bool Contain(const wstring & InValue, const wstring & InCompare);

	static void Replace(string* InValue, const string & InCompare, const string & InReplace);
	static void Replace(wstring* InValue, const wstring & InCompare, const wstring & InReplace);

	static wstring ToWString( const string & Value);
	static string ToString(const wstring & InValue);

	static string Format(string InFormat, ...);
	static wstring Format(wstring InFormat, ...);
};
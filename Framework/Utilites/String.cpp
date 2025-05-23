#include "Framework.h"
#include "String.h"

#include <codecvt>

void String::SplitString(vector<string>* InResult, const string & InOrigin, const string & InToken)
{
	InResult->clear();

	size_t StartIndex = 0;

	while (true)
	{
		const size_t EndIndex = InOrigin.find(InToken, StartIndex);
		if (EndIndex == string::npos)
		{
			break;
		}
		if (EndIndex > StartIndex)
		{
			InResult->emplace_back(InOrigin.substr(StartIndex, EndIndex - StartIndex));
		}
		StartIndex = EndIndex + InToken.length(); // 토큰 길이만큼 건너뜀
	}

	if (StartIndex < InOrigin.size())
	{
		InResult->emplace_back(InOrigin.substr(StartIndex));
	}
}

void String::SplitString(vector<wstring>* InResult, const wstring & InOrigin, const wstring & InToken)
{
	InResult->clear();

	size_t StartIndex = 0;

	while (true)
	{
		const size_t EndIndex = InOrigin.find(InToken, StartIndex);
		if (EndIndex == wstring::npos)
		{
			break;
		}
		if (EndIndex > StartIndex)
		{
			InResult->emplace_back(InOrigin.substr(StartIndex, EndIndex - StartIndex));
		}
		StartIndex = EndIndex + InToken.length();
	}

	if (StartIndex < InOrigin.size())
	{
		InResult->emplace_back(InOrigin.substr(StartIndex));
	}
}


bool String::StartsWith(const string & InValue, const string & InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool String::StartsWith(const wstring & InValue, const wstring & InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool String::Contain(const string & InValue, const string & InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}

bool String::Contain(const wstring & InValue, const wstring & InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}


void String::Replace(string * InValue, const string & InCompare, const string & InReplace)
{
	string temp = *InValue;

	size_t start_pos = 0;
	while ((start_pos = temp.find(InCompare, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, InCompare.length(), InReplace);
		start_pos += InReplace.length();
	}

	*InValue = temp;
}

void String::Replace(wstring* InValue, const wstring & InCompare, const wstring & InReplace)
{
	wstring Temp = *InValue;

	size_t start_pos = 0;
	while ((start_pos = Temp.find(InCompare, start_pos)) != wstring::npos)
	{
		Temp.replace(start_pos, InCompare.length(), InReplace);
		start_pos += InReplace.length();
	}

	*InValue = Temp;
}

wstring String::ToWString( const string & Value)
{
	wstring temp = L"";
	temp.assign(Value.cbegin(), Value.cend());

	return temp;
}

string String::ToString(const wstring & InValue)
{
	wstring_convert<codecvt_utf8<wchar_t>> Converter;

	return Converter.to_bytes(InValue);
}

// ReSharper disable once CppPassValueParameterByConstReference
string String::Format(const string InFormat, ...)
{
	va_list Args;

	// va_start의 두 번째 인자는 반드시 함수의 매개변수 이름이여야한다.
	// va_start는 내부적으로 해당 변수의 주소를 기반으로 va_list를 초기화 한다.
	// string InFormat을 const string & InFormat으로 바꾸는건 안 될까?
	// -> [!] va_start()의 두 번쨰 인자는 반드시 값타입이여야 한다. [!]
	// 참조로 받고 CstrFormat을 전달하면 안 될까?
	// -> [!] CstrFormat은 함수의 매개변수가 아니기 떄문에 안 된다. [!]
	va_start(Args, InFormat);
	va_list ArgsCopy;
	va_copy(ArgsCopy, Args);
	const char * CstrFormat = InFormat.c_str();
	const size_t Len = vsnprintf(nullptr, 0, CstrFormat, ArgsCopy);
	va_end(ArgsCopy);
	
	string ResultStr(Len, 0);
	
	vsnprintf(&(ResultStr[0]), Len + 1, CstrFormat, Args);
	va_end(Args);

	return ResultStr;
}

// ReSharper disable once CppPassValueParameterByConstReference
wstring String::Format(const wstring InFormat, ...)
{
	va_list Args;
	va_start(Args, InFormat);
	va_list ArgsCopy;
	va_copy(ArgsCopy, Args);
	const wchar_t * CwstrFormat = InFormat.c_str();
	const size_t Len = _vsnwprintf_s(nullptr, 0, _TRUNCATE, CwstrFormat, ArgsCopy);
	va_end(ArgsCopy);

	wstring ResultStr(Len, 0);
	
	_vsnwprintf_s(&ResultStr[0], Len + 1, _TRUNCATE, CwstrFormat, Args);
	va_end(Args);

	return ResultStr;
}
#include "Framework.h"
#include "String.h"

#include <codecvt>
#include <cvt/wstring>


//////////////////////////////////////////////////////////////////////////
///@brief ���ڿ� �ڸ���
///@param InOrigin : ���� ���ڿ�
///@param InToken : �ڸ� ������ �Ǵ� ���ڿ�
///@return �Ϸ�� ���ڿ� �迭
//////////////////////////////////////////////////////////////////////////
void String::SplitString(vector<string>* InResult, string InOrigin, string InToken)
{
	InResult->clear();

	int CutAt; //�ڸ� ��ġs
	while ((CutAt = static_cast<int>(InOrigin.find_first_of(InToken))) != InOrigin.npos)
	{
		if (CutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			InResult->push_back(InOrigin.substr(0, CutAt));

		InOrigin = InOrigin.substr(CutAt + 1);
	}

	if (InOrigin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		InResult->push_back(InOrigin.substr(0, CutAt));
}

void String::SplitString(vector<wstring>* InResult, wstring InOrigin, wstring InToken)
{
	InResult->clear();

	int CutAt; //�ڸ� ��ġs
	while ((CutAt = static_cast<int>(InOrigin.find_first_of(InToken))) != InOrigin.npos)
	{
		if (CutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			InResult->push_back(InOrigin.substr(0, CutAt));

		InOrigin = InOrigin.substr(CutAt + 1);
	}

	if (InOrigin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		InResult->push_back(InOrigin.substr(0, CutAt));
}


//////////////////////////////////////////////////////////////////////////
///@brief ���� ���ڿ��� ������ üũ
///@param InValue : üũ�Ϸ��� ���ڿ�
///@param InCompare : ���� �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool String::StartsWith(string InValue, string InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool String::StartsWith(wstring InValue, wstring InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ��� ���ԵǾ� �ִ���
///@param InValue : üũ�Ϸ��� ���ڿ�
///@param InCompare : �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool String::Contain(string InValue, string InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}

bool String::Contain(wstring InValue, wstring InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ����� InCompare ���ڸ� InReplace�� ����
///@param InValue : üũ�Ϸ��� ���ڿ�
///@param InCompare : �񱳹��ڿ�
///@param InReplace : �ٲܹ��ڿ�
//////////////////////////////////////////////////////////////////////////
void String::Replace(string * InValue, string InCompare, string InReplace)
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

void String::Replace(wstring* InValue, wstring InCompare, wstring InReplace)
{
	wstring temp = *InValue;

	size_t start_pos = 0;
	while ((start_pos = temp.find(InCompare, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, InCompare.length(), InReplace);
		start_pos += InReplace.length();
	}

	*InValue = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string���� wstring������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
wstring String::ToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring���� string������ ����
///@param InValue : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
string String::ToString(const wstring & InValue)
{
	wstring_convert<codecvt_utf8<wchar_t>> Converter;

	return Converter.to_bytes(InValue);
}

string String::Format(const string InFormat, ...)
{
	va_list args;

	va_start(args, InFormat);
	size_t Len = vsnprintf(nullptr, 0, InFormat.c_str(), args);
	va_end(args);

	vector<char> vec(Len + 1);

	va_start(args, InFormat);
	vsnprintf(&vec[0], Len + 1, InFormat.c_str(), args);
	va_end(args);

	return &vec[0];
}

wstring String::Format(const wstring format, ...)
{
	va_list args;

	va_start(args, format);
	// size_t len = _vsnwprintf(NULL, 0, format.c_str(), args);
	size_t len = _vsnwprintf_s(nullptr, 0, _TRUNCATE, format.c_str(), args);
	va_end(args);

	vector<WCHAR> vec(len + 1);

	va_start(args, format);
	// _vsnwprintf(&vec[0], len + 1, format.c_str(), args);
	_vsnwprintf_s(&vec[0], len + 1, _TRUNCATE, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

//////////////////////////////////////////////////////////////////////////
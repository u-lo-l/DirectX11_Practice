#pragma once

class BinaryWriter
{
public:
	explicit BinaryWriter(const wstring & InFilePath, UINT InOption = CREATE_ALWAYS);
	~BinaryWriter();
	
	void Close();
	
public:
	void Write( const void * InData, UINT InDataSize) const;
private:
	HANDLE FileHandle;
};

#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();
	
	void Close();
	
public:
	void Open(const wstring & InFilePath, UINT InOption = CREATE_ALWAYS);

	void WriteUint(UINT InData) const;
	void WriteInt(INT InData) const;
	void WriteString(const string & InData) const;
	void WriteMatrix(const Matrix & InMatrix) const;
	void WriteByte( const void * InData, UINT InDataSize) const;
private:
	HANDLE FileHandle;
};

#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();
	
	void Open(const wstring & InFilePath, UINT InOption = CREATE_ALWAYS);
	void Close();

	void WriteUint(UINT InData) const;
	void WriteInt(INT InData) const;
	void WriteString(const string & InData) const;
	void WriteMatrix(const Matrix & InMatrix) const;
	void WriteByte( const void * InData, UINT InDataSize) const;
private:
	HANDLE FileHandle;
};

/*-----------------------------------------------------------------------------*/

class BinaryReader
{
public:
	BinaryReader();
	~BinaryReader();
	
	bool Open(const wstring & InFilePath);
	void Close();

	UINT ReadUint() const;
	INT ReadInt() const;
	string ReadString() const;
	Matrix ReadMatrix() const;
	void ReadByte( void ** OutData, UINT InDataSize) const;

private:
	HANDLE FileHandle;
};
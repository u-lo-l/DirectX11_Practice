#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	explicit BinaryWriter(const string & InFileName);
	~BinaryWriter();
	
	void Open( const string & InFilePath, UINT InOption = CREATE_ALWAYS);
	void Open(const wstring & InFilePath, UINT InOption = CREATE_ALWAYS);
	void Close();

	void WriteUint(UINT InData) const;
	void WriteInt(INT InData) const;
	void WriteFloat(float InData) const;
	void WriteString(const string & InData) const;
	void WriteMatrix(const Matrix & InMatrix) const;
	template<typename T>
	void WriteSTDVector(const vector<T>& InVector) const;
	void WriteByte( const void * InData, UINT InDataSize) const;

private:
	HANDLE FileHandle;
};

template <typename T>
void BinaryWriter::WriteSTDVector( const vector<T> & InVector ) const
{
	const UINT DataCount = InVector.size();
	const UINT DataStride = sizeof(T);
	const UINT DataSize = DataCount * DataStride;
	WriteUint(DataCount);
	if (DataSize > 0)
		WriteByte(InVector.data(), DataSize);
}

/*-----------------------------------------------------------------------------*/

class BinaryReader
{
public:
	BinaryReader();
	explicit BinaryReader( const wstring & InFilePath );
	~BinaryReader();
	
	bool Open(const wstring & InFilePath);
	void Close();

	UINT ReadUint() const;
	INT ReadInt() const;
	float ReadFloat() const;
	string ReadString() const;
	Matrix ReadMatrix() const;
	void ReadByte( void ** OutData, UINT InDataSize) const;

private:
	HANDLE FileHandle;
	wstring FileName;
};
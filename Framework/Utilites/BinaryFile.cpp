#include "framework.h"
#include "BinaryFile.h"

BinaryWriter::BinaryWriter()
: FileHandle(nullptr)
{
	
}

BinaryWriter::BinaryWriter( const string & InFileName )
{
	Open(InFileName);
}

BinaryWriter::~BinaryWriter()
{
	Close();
}

void BinaryWriter::Close()
{
	if (FileHandle == nullptr)
		return;
	CloseHandle(FileHandle);
	FileHandle = nullptr;
}
void BinaryWriter::Open( const string & InFilePath, UINT InOption)
{
	ASSERT(InFilePath.empty() == false, "[BinaryWriter] Wrong File Path");

	FileHandle = CreateFileA(
		InFilePath.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		InOption,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	ASSERT(FileHandle != INVALID_HANDLE_VALUE, "[BinaryWriter] Failed to create file");
}
void BinaryWriter::Open( const wstring & InFilePath, UINT InOption )
{
	ASSERT(InFilePath.empty() == false, "[BinaryWriter] Wrong File Path");

	FileHandle = CreateFileW(
		InFilePath.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		InOption,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	ASSERT(FileHandle != INVALID_HANDLE_VALUE, "[BinaryWriter] Failed to create file");
}

void BinaryWriter::WriteUint( UINT InData ) const
{
	DWORD BytesWritten = 0;
	const bool Result = WriteFile(FileHandle, &InData, sizeof(UINT), &BytesWritten, nullptr);
	ASSERT((Result == true), "[BinaryWriter] Failed to write to file to UINT");
}

void BinaryWriter::WriteInt( INT InData ) const
{
	DWORD BytesWritten = 0;
	const bool Result = WriteFile(FileHandle, &InData, sizeof(UINT), &BytesWritten, nullptr);
	ASSERT((Result == true), "[BinaryWriter] Failed to write to file to UINT");
}

void BinaryWriter::WriteFloat( float InData ) const
{
	DWORD BytesWritten = 0;
	const bool Result = WriteFile(FileHandle, &InData, sizeof(float), &BytesWritten, nullptr);
	ASSERT((Result == true), "[BinaryWriter] Failed to write to file to float");
}

void BinaryWriter::WriteString( const string & InData ) const
{
	DWORD BytesWritten = 0;
	WriteUint(InData.size());

	const char * const Str = InData.c_str();
	const bool Result = WriteFile(FileHandle, Str, InData.size(), &BytesWritten, nullptr);
	ASSERT((Result == true) && (BytesWritten == InData.size()), "[BinaryWriter] Failed to write to file to string");

}

void BinaryWriter::WriteMatrix( const Matrix & InMatrix ) const
{
	DWORD BytesWritten = 0;
	
	const float * const MatrixInRow = InMatrix;
	const bool Result = WriteFile(FileHandle, MatrixInRow, sizeof(Matrix), &BytesWritten, nullptr);
	ASSERT((Result == true) && (BytesWritten == sizeof(Matrix)), "[BinaryWriter] Failed to write to file to Matrix");
}

void BinaryWriter::WriteByte( const void * InData, UINT InDataSize ) const
{
	if (InDataSize == 0)
		return ;
	DWORD BytesWritten = 0;
	const bool Result = WriteFile(FileHandle, InData, InDataSize, &BytesWritten, nullptr);
	ASSERT((Result == true) && (BytesWritten == InDataSize), "[BinaryWriter] Failed to write to file");
}

/*-----------------------------------------------------------------------------*/

BinaryReader::BinaryReader()
: FileHandle(nullptr)
{
}

BinaryReader::BinaryReader(const wstring & InFilePath)
: FileHandle(nullptr)
{
	Open(InFilePath);
}

BinaryReader::~BinaryReader()
{
	Close();
}

bool BinaryReader::Open( const wstring & InFilePath )
{
	ASSERT(FileHandle == nullptr, "[BinaryReader] BinaryReader In Use");
	ASSERT(InFilePath.empty() == false, "[BinaryRead] Wrong File Path");

	FileName = InFilePath;
	FileHandle = CreateFile(
		InFilePath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	
	return (FileHandle != INVALID_HANDLE_VALUE);
	ASSERT(FileHandle != INVALID_HANDLE_VALUE, "[BinaryRead] Failed to create file");
}

void BinaryReader::Close()
{
	if (FileHandle == nullptr)
		return;
	CloseHandle(FileHandle);
	FileHandle = nullptr;
	FileName = L"";
}

UINT BinaryReader::ReadUint() const
{
	UINT UintBuffer;
	DWORD ByteRead = 0;
	
	const bool Result = ReadFile(FileHandle, &UintBuffer, sizeof(UINT), &ByteRead, nullptr);
	ASSERT((Result == true) && (ByteRead == sizeof(UINT)), "[BinaryReader] Failed to read UINT from file");
	
	return UintBuffer;
}

INT BinaryReader::ReadInt() const
{
	INT IntBuffer;
	DWORD ByteRead = 0;
	
	const bool Result = ReadFile(FileHandle, &IntBuffer, sizeof(INT), &ByteRead, nullptr);
	ASSERT((Result == true) && (ByteRead == sizeof(INT)), "[BinaryReader] Failed to read INT from file");
	
	return IntBuffer;
}

float BinaryReader::ReadFloat() const
{
	float FloatBuffer;
	DWORD ByteRead = 0;
	
	const bool Result = ReadFile(FileHandle, &FloatBuffer, sizeof(float), &ByteRead, nullptr);
	ASSERT((Result == true) && (ByteRead == sizeof(float)), "[BinaryReader] Failed to read float from file");
	
	return FloatBuffer;
}

string BinaryReader::ReadString() const
{
	const UINT StringLength = ReadUint();
	char * StrBuffer = new char[StringLength + 1];
	DWORD ByteRead = 0;
	
	const bool Result = ReadFile(FileHandle, StrBuffer, StringLength, &ByteRead, nullptr);
	ASSERT((Result == true) && (ByteRead == StringLength), "[BinaryReader] Failed to read string from file");

	StrBuffer[StringLength] = '\0';
	string ReturnStr(StrBuffer);
	SAFE_DELETE(StrBuffer);
	
	return ReturnStr;
}

Matrix BinaryReader::ReadMatrix() const
{
	DWORD ByteRead = 0;
	Matrix MatrixBuffer;
	
	const bool Result = ReadFile(FileHandle, &MatrixBuffer, sizeof(Matrix), &ByteRead, nullptr);
	ASSERT((Result == true) && (ByteRead == sizeof(Matrix)), "[BinaryReader] Failed to read Matrix from file");

	return MatrixBuffer;
}

void BinaryReader::ReadByte( void ** OutData, UINT InDataSize ) const
{
	if (InDataSize == 0)
		return ;
	DWORD BytesRead = 0;
	const bool Result = ReadFile(FileHandle, *OutData, InDataSize, &BytesRead, nullptr);
	ASSERT((Result == true) && (BytesRead == InDataSize), "[BinaryWriter] Failed to read bytes from file");
}
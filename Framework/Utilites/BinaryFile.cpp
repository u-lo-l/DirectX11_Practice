#include "framework.h"
#include "BinaryFile.h"

BinaryWriter::BinaryWriter()
: FileHandle(nullptr)
{
	
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

void BinaryWriter::Open( const wstring & InFilePath, UINT InOption )
{
	ASSERT(InFilePath.empty() == false, "[BinaryWriter] Wrong File Path");

	FileHandle = CreateFile(
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

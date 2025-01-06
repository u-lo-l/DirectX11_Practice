#include "framework.h"
#include "BinaryFile.h"

BinaryWriter::BinaryWriter( const wstring & InFilePath, UINT InOption )
: FileHandle(nullptr)
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

void BinaryWriter::Write( const void * InData, UINT InDataSize ) const
{
	DWORD BytesWritten = 0;
	const bool Result = WriteFile(FileHandle, InData, InDataSize, &BytesWritten, nullptr);
	ASSERT((Result == true) && (BytesWritten == InDataSize), "[BinaryWriter] Failed to write to file");
}

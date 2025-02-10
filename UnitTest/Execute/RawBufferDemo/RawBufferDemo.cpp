#include "Pch.h"
#include "RawBufferDemo.h"

namespace Sdt
{
	const wstring ShaderName = L"39_RawBuffer.fx";
	void RawBufferDemo::Initialize()
	{
		Shader * shader = new Shader(ShaderName);

		UINT ThreadCount = 10 * 8 * 3;
		struct Output
		{
			UINT GroupId[3];
			UINT ThreadId[3];
			UINT DispatchThreadId[3];
			UINT GroupIndex;
			float Result;
		};

		RawBuffer * RBuffer = new RawBuffer(nullptr, "Raw Buffer Test", 0, sizeof(Output) * ThreadCount);
		shader->AsUAV("Output")->SetUnorderedAccessView(*RBuffer);
		shader->Dispatch(0, 0, 1, 1, 1);
		
		Output * outputs = new Output[ThreadCount];
		RBuffer->GetOutputData(outputs);

		FILE* file;
		fopen_s(&file, "../Raw.csv", "w");
		fprintf(file,
			"ThreadIndex,GroupId(X Y Z),ThreadId(X Y Z),DispatchThreadId(X Y Z),GroupID\n"
		);
		for (UINT i = 0 ;i < ThreadCount ; i++)
		{
			const Output & Temp = outputs[i];
			fprintf(
					 file,
					 "%d,<%d %d %d>,<%d %d %d>,<%d %d %d>,%d\n",
					 i,
					 Temp.GroupId[0], Temp.GroupId[1], Temp.GroupId[2],
					 Temp.ThreadId[0], Temp.ThreadId[1], Temp.ThreadId[2],
					 Temp.DispatchThreadId[0], Temp.DispatchThreadId[1], Temp.DispatchThreadId[2],
					 Temp.GroupIndex
				  );
		}
		
		fclose(file);
		
		SAFE_DELETE_ARR(outputs);
		SAFE_DELETE(shader)
	}

	void RawBufferDemo::Destroy()
	{
	}
}

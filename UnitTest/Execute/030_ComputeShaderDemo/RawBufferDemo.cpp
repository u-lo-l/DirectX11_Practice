#include "Pch.h"
#include "RawBufferDemo.h"

#include <fstream>

namespace sdt
{
	void RawBufferDemo::Initialize()
	{
		ComputeShader = new HlslComputeShader(L"39_ComputeShaderDemo.hlsl", nullptr, "CSMain");

		UINT count = (3 * 2 * 1) * (5 * 4 * 1); //Dispatch Dimension X numthread Dimension

		InputDesc* input = new InputDesc[count];
	
		for (UINT i = 0; i < count; i++)
		{
			input[i].Index = i;
			input[i].Value = Math::Random(0.0f, (float)count);
		}

		// SRV, UAV 내부에서 생성
		RawBuffer * rawBuffer = new RawBuffer(input, sizeof(InputDesc) * count, sizeof(OutputDesc) * count);
		
		ComputeShader->Dispatch(rawBuffer,3, 2, 1);

		OutputDesc * output = new OutputDesc[count];
		rawBuffer->GetOutputData(output);

		ofstream file;
		file.open("../ComputeShaderTest.csv");
		file << "GroupID,GroupIndex,ThreadID,DispatchID,TotalIndex,Value1,Value" << std::endl; 
		for (UINT i = 0; i < count; i++)
		{
			file << "[" << output[i].GroupID[0] << " | " <<output[i].GroupID[1] << " | " <<output[i].GroupID[2] << "],";

			file << output[i].Index << ",";

			file << "[" << output[i].ThreadID[0] << " | " <<output[i].ThreadID[1] << " | " <<output[i].ThreadID[2] << "],";
			file << "[" << output[i].DispatchID[0] << " | " <<output[i].DispatchID[1] << " | " <<output[i].DispatchID[2] << "],";

			file << output[i].TotalIndex << ",";
			file << output[i].Value << ",";
			file << output[i].Value2 << endl;
		}
		file.close();
	}
}

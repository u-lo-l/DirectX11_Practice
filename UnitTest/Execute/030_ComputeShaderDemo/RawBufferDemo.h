#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class RawBufferDemo final : public IExecutable
	{
	public:
		void Initialize() override;
	private:
		struct InputDesc
		{
			UINT Index;
			float Value;
		};
		struct OutputDesc
		{
			UINT GroupID[3];

			UINT Index;
			UINT ThreadID[3];
			UINT DispatchID[3];

			UINT TotalIndex;
			float Value;
			float Value2;
		};
		HlslComputeShader * ComputeShader = nullptr;
	};
}

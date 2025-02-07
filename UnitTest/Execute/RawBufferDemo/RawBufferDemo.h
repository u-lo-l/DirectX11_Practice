#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class RawBufferDemo final : public IExecutable
	{

	public:
		void Initialize() override;
		void Destroy() override;
	};
}

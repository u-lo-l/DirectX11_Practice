#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class Convert final : public IExecutable
	{
	public:
		void Initialize() override;

	private:
		void Airplane();
		void Cube();
	};
}

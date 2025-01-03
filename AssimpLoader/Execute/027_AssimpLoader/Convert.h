#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class Convert final : public IExecutable
	{
	public:
		using VertexType = VertexColor;
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void Render() override;
	private:

	};
}

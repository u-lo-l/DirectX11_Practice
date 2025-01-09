#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class DrawModel final : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;
	private:
		Model * Cube;
		Shader * Drawer;
	};
}

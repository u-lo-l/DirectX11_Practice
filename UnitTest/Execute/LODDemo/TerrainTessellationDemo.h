#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class TerrainTessellationDemo final : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		
		void Tick() override;
		void Render() override;
	private:
		Terrain2 * Terrain = nullptr;
	};
}

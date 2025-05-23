#pragma once
#include "Systems/IExecutable.h"

namespace sdt
{
	class CellDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Tick() override;
		void Render() override;
		void Destroy() override;
	private:
		LandScape * Terrain = nullptr;
		Ocean * Sea = nullptr;
		SkySphere * Sky = nullptr;
		bool bDrawTerrain = true;
		bool bDrawSea = true;
	};
	
}

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
	bool bRenderCell = false;
	LandScape_QuadTree * TerrainWidthCell = nullptr;
	LandScape * TerrainNoCell = nullptr;
	SkySphere * Sky = nullptr;
	};
	
}

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
	LandScape * TerrainWidthCell = nullptr;
	SkySphere * Sky = nullptr;
	};
	
}

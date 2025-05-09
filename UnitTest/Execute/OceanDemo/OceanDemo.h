#pragma once

#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class OceanDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;

	private:
		Ocean * Sea = nullptr;
		SkySphere * Sky = nullptr;
		LandScape * Terrain = nullptr;
		Foliage * Grasses = nullptr;
	};
}

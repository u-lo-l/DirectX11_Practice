#pragma once

#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class LandScapeDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;

	private:
		SkySphere * Sky = nullptr;
		LandScape * Terrain = nullptr;
		Foliage * Grasses = nullptr;
	};
}

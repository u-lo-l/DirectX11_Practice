#pragma once

#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class FoamDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;

	private:
		Ocean * Sea = nullptr;
		SkySphere * Sky = nullptr;
		const RWTexture2D * DisplacementGrid = nullptr;
		vector<Hlsl2DTextureShader*> TextureDebugShaders;
	};
}

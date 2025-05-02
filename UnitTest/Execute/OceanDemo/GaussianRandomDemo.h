#pragma once

#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class GaussianRandomDemo : public IExecutable
	{
	public:
		static constexpr int Size = 2048;
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;

	private:
		Texture * GaussianRandom = nullptr;
		Hlsl2DTextureShader * Shader = nullptr;
	};
}

#pragma once

#include <complex>

#include "Systems/IExecutable.h"

class Hlsl2DTextureShader;

namespace sdt
{
	class GaussianRandomDemo : public IExecutable
	{
	private:
	public:
		static constexpr int Size = 256;
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;

	private:
		Texture * GaussianRandom = nullptr;
		Hlsl2DTextureShader * Shader = nullptr;
	};
}

#pragma once

#include <complex>

#include "Systems/IExecutable.h"

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

		array<array<std::complex<float>, Size>, Size> GaussianRandomArray = {};
	};
}

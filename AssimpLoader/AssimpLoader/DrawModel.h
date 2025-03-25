#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	// NOLINT(cppcoreguidelines-special-member-functions)
	class DrawModel final : public IExecutable
	{
	public:
		~DrawModel() override;
		void Initialize() override;
		void Destroy() override;
		void Tick() override;
		void Render() override;
	private:
		vector<Model *> Models;
		int ModelIndex = 0;
	};
}

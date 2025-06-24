#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	// NOLINT(cppcoreguidelines-special-member-functions)
	class DrawModel final : public IExecutable
	{
	public:
		virtual ~DrawModel() override;
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void Tick() override;
		virtual void Render() override;
	private:
		// vector<Model *> Models;
		int ModelIndex = 0;
	};
}

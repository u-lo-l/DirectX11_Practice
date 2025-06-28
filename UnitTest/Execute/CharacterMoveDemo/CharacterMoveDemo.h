#pragma once
#include "Systems/IExecutable.h"

class Character;

namespace sdt
{
	class CharacterMoveDemo : public IExecutable
	{
	public:
		virtual void Initialize() override;
		virtual void Destroy() override;

		virtual void Tick() override;
		virtual void PreRender() override;
		virtual void Render() override;
		virtual void PostRender() override;

	private:
		void SetTerrain();
		void SetCharacter();
		
		LandScape * Terrain = nullptr;
		Character * Adam = nullptr;
	};
}

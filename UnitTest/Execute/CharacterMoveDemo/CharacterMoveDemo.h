#pragma once
#include "Systems/IExecutable.h"

class Character;

namespace sdt
{
	class CharacterMoveDemo : public IExecutable
	{
	public:
		void Initialize() override;
		void Destroy() override;

		void Tick() override;
		void PreRender() override;
		void Render() override;
		void PostRender() override;

	private:
		void SetTerrain();
		void SetCharacter();
		
		LandScape * Terrain = nullptr;
		Character * Adam = nullptr;
	};
}

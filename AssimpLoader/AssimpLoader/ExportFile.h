#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		void Initialize() override;
		static void MakeModelInfoFile( const wstring & InModelName );

	private:
		void Airplane();
		void Cube();
		void Shannon();
		void Mousey();
	};
}

#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		void Initialize() override;
		static void MakeModelInfoFile( const wstring & InModelName, const vector<wstring> & InAnimationNames );

	private:
		static void Airplane();
		static void Cube();
		static void Sphere();
		static void Shannon();
		static void Mousey();
		static void Manny();
		static void YBot();
		static void XYBot();
		static void Example();
	};
}

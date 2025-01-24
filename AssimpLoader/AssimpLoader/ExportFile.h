#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		void Initialize( ) override;
	private:
		static void MakeModel( const wstring & InModelName, const vector<wstring> & InAnimationNames, float InScale = 1.f);
		static void MakeModelInfoFile( const wstring & InModelName, const vector<wstring> & InAnimationNames, float InScale = 1.f );
	};
}

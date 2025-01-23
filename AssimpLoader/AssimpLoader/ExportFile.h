#pragma once

#include "Systems/IExecutable.h"

namespace Sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		void Initialize( ) override;
	private:
		static void MakeModel( const wstring & InModelName, const vector<wstring> & InAnimationNames );
		static void MakeModelInfoFile( const wstring & InModelName, const vector<wstring> & InAnimationNames );
	};
}

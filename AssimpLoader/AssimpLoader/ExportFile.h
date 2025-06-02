#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		ExportFile();
		void Initialize( ) override;
	private:
		static void ExportMeshes(const vector<wstring> & FBXFileNames);
		static void ExportAnimations(const vector<wstring> & FBXFileNames);
		static void MakeModel( const wstring & InModelName, const vector<wstring> & InAnimationNames = {}, float InScale = 1.f);
		static void MakeModelInfoFile( const wstring & InModelName, const vector<wstring> & InAnimationNames = {}, float InScale = 1.f );
	};
}

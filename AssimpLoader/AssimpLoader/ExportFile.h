#pragma once

#include "Systems/IExecutable.h"

namespace sdt
{
	class ExportFile final : public IExecutable
	{
	public:
		ExportFile();
		virtual ~ExportFile() override;
		virtual void Initialize( ) override;
	private:
		static void ExportMeshes(const vector<wstring> & FBXFileNames);
		static void ExportAnimations(const vector<wstring> & FBXFileNames);
	};
}

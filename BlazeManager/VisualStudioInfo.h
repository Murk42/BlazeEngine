#pragma once
#include <string>
#include "Result.h"

struct VisualStudioInfo
{
	std::string instalationPath;

	std::string devenvPath;
	std::string MSBuildPath;
};

Result GetVisualStudioInfo(VisualStudioInfo&);

void PrintVisualStudioInfo(VisualStudioInfo info);
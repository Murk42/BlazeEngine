#pragma once
#include <string>

struct VisualStudioInfo
{
	std::string instalationPath;

	std::string devenvPath;
	std::string MSBuildPath;
};

VisualStudioInfo GetVisualStudioInfo();

void PrintVisualStudioInfo(VisualStudioInfo info);
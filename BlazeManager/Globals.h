#pragma once
#include <string>
#include <unordered_map>

#include "VisualStudioInfo.h"
#include "Build.h"

extern VisualStudioInfo vsInfo;

extern string engineProjectDir;
extern string engineDLLPath;
extern string engineLIBPath;
extern unordered_map<Configuration, vector<string>> engineDependencies;

extern string runtimeProjectDir;
extern string runtimeDLLPath;
extern string runtimeLIBPath;
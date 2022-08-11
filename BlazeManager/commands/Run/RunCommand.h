#pragma once
#include <string>
using namespace std;

struct RunCommandOptions
{
	bool managerLog = true;
	bool runtimeLog = true;
	bool logTimings = true;
	bool release = false;
	bool stopAfter = false;
	bool dontBuildClient = true;
	bool dontBuildBlaze = true;
	bool dontBuildRuntime = true;
	bool guiOpenFile = false;
	string projectPath = "";
};

bool RunCommand(RunCommandOptions options);
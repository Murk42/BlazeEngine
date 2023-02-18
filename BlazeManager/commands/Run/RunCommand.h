#pragma once
#include <string>
using namespace std;

#include "Result.h"
#include "Build.h"

enum class RunCommandExitCondition
{
	NoExit,
	ForceExit
};

struct RunCommandOptions
{
	bool managerLog = true;
	bool runtimeLog = true;
	bool logTimings = false;

	RuntimeConfiguration configuration = RuntimeConfiguration::Debug;
	RunCommandExitCondition exitCondition = RunCommandExitCondition::NoExit;
	
	bool buildClient = false;
	bool buildEngine = false;
	bool buildRuntime = false;

	string projectPath = "";
};

Result RunCommand(RunCommandOptions options);
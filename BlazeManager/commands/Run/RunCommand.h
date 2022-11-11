#pragma once
#include <string>
using namespace std;

enum class RunCommandConfiguration
{
	Debug,
	Release
};
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

	RunCommandConfiguration configuration = RunCommandConfiguration::Debug;
	RunCommandExitCondition exitCondition = RunCommandExitCondition::NoExit;
	
	bool buildClient = false;
	bool buildBlaze = false;
	bool buildRuntime = false;

	string projectPath = "";
};

bool RunCommand(RunCommandOptions options);
#pragma once
#include <string>
using namespace std;

struct RunCommandOptions
{
	bool release;
	bool stopAfter;
	bool dontBuildClient;
	bool dontBuildBlaze;
	bool dontBuildRuntime;		
	string projectPath;
};

bool RunCommand(RunCommandOptions options);
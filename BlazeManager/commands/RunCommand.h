#pragma once
#include <string>
using namespace std;

struct RunCommandOptions
{
	bool log;
	bool release;
	bool stopAfter;
	bool dontBuildClient;
	bool dontBuildBlaze;
	bool dontBuildRuntime;		
	bool guiOpenFile;
	string projectPath;
};

bool RunCommand(RunCommandOptions options);
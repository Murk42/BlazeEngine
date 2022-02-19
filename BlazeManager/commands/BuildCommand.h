#pragma once
#include <string>
using namespace std;
#include "../Build.h"

struct BuildCommandOptions
{
	Configuration configuration;
	Platform platform;
	string projectPath;

	string outputDir;
	string outputName;
};

bool BuildCommand(BuildCommandOptions options);
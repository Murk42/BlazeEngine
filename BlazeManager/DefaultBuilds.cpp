#include "DefaultBuilds.h"
#include <iostream>

extern VisualStudioInfo vsInfo;
extern string blazeDir;
extern string runtimeDir;

bool BuildBlaze(Configuration configuration, Platform platform)
{
	BuildSettings blazeBuildSettings;
	blazeBuildSettings.projectPath = blazeDir + "BlazeEngine.vcxproj";
	blazeBuildSettings.platform = platform;
	blazeBuildSettings.configuration = configuration;
	blazeBuildSettings.outputDir = blazeDir + GetOutputSubDir(configuration, platform);
	blazeBuildSettings.intermediateDir = blazeDir + GetIntermediateSubDir(configuration, platform);

	Result blazeBuildResult = BuildProject(vsInfo, blazeBuildSettings, "-p:WarningLevel=0");

	if (!blazeBuildResult.sucessfull)
	{
		cout << blazeBuildResult.log;
		return true;
	}
	return false;
}
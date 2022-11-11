#pragma once
#include "Build.h"

struct ClientLibraryInfo
{
	std::vector<std::string> additionalLibraryDirectories;
	std::vector<std::string> additionalDependencies;
	std::vector<std::string> additionalDynamicLibraries;
};

struct EngineBuildSettings
{
	std::string projectPath;
	std::string outputDir;
};

struct ClientBuildSettings
{
	std::string projectPath;
	std::string outputDir;
	std::string enginePathLIB;	
};
struct RuntimeBuildSettings
{
	std::string projectPath;
	std::string outputDir;

	std::string clientOutputDir;	
	ClientLibraryInfo clientLibraryInfo;
};

Result LoadClientLibraryInfo(const string& path, bool log, ClientLibraryInfo& info);

bool BuildBlaze(Configuration configuration, Platform platform, EngineBuildSettings settings);
bool BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings);
bool BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings);
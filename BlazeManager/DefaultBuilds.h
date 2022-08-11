#pragma once
#include "Build.h"

struct ClientLibraryInfo
{
	std::vector<std::string> additionalLibraryDirectories;
	std::vector<std::string> additionalDependencies;
	std::vector<std::string> additionalDynamicLibraries;
};

struct ClientBuildSettings
{
	std::string projectPath;
	std::string outputDir;
};
struct RuntimeBuildSettings
{
	std::string clientOutputDir;
	std::string outputDir;
	
	ClientLibraryInfo clientLibraryInfo;
};

Result LoadClientLibraryInfo(const string& path, bool log, ClientLibraryInfo& info);

bool BuildBlaze(Configuration configuration, Platform platform);
bool BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings);
bool BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings);
#pragma once
#include "Build.h"

struct ClientLibraryInfo
{
	std::vector<std::string> assetsPaths;
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

	std::string engineLibraryDir;
};
struct RuntimeBuildSettings
{
	std::string projectPath;
	std::string outputDir;

	std::string clientLibraryDir;
};

Result LoadClientLibraryInfo(const string& path, bool log, Configuration configuration, ClientLibraryInfo& info);

Result BuildBlaze(Configuration configuration, Platform platform, EngineBuildSettings settings, std::string& result);
Result BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings, std::string& result);
Result BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings, std::string& result);
#pragma once
#include <string>
#include "VisualStudioInfo.h"
#include "Command.h"

enum class Configuration
{
	Debug,
	Debug_NoPDB,
	Release,
	FinalBuild_Debug,
	FinalBuild_Release,
};
enum class Platform
{
	x86,
	x64
};
enum class BuildOutputType
{
	StaticLibrary,
	DynamicLibrary,
	Application
};

constexpr Configuration GetConfiguration()
{
#if defined(DEBUG)
	return Configuration::Debug;
#else
	return Configuration::Release;
#endif
}
constexpr Platform GetPlatform()
{
#if defined(X86)
	return Platform::x86;
#else
	return Platform::x64;
#endif
}

constexpr Configuration configuration = GetConfiguration();
constexpr Platform platform = GetPlatform();

constexpr const char* GetConfigurationString(Configuration configuration)
{
	switch (configuration)
	{
	case Configuration::Debug: return "Debug"; break;
	case Configuration::Debug_NoPDB: return "Debug_NoPDB"; break;
	case Configuration::Release: return "Release"; break;
	case Configuration::FinalBuild_Debug: return "FinalBuild_Debug"; break;
	case Configuration::FinalBuild_Release: return "FinalBuild_Release"; break;
	}
}
constexpr const char* GetPlatformString(Platform platform)
{
	switch (platform)
	{
	case Platform::x86: return "x86"; break;
	case Platform::x64: return "x64"; break;
	}
}
constexpr const char* GetBuildOutputTypeString(BuildOutputType outputType)
{
	switch (outputType)
	{
	case BuildOutputType::StaticLibrary: return "StaticLibrary";
	case BuildOutputType::DynamicLibrary: return "DynamicLibrary";
	case BuildOutputType::Application: return "Application";
	}
}

struct BuildSettings
{
	std::string projectPath;
	std::string outputDir;
	std::string outputName;
	std::string intermediateDir;
	std::string additionalLibraryDirectories;
	std::string additionalDependencies;
	Configuration configuration;
	Platform platform;
	BuildOutputType outputType;
};

Result BuildProject(VisualStudioInfo vsInfo, BuildSettings settings, std::string additional = "");
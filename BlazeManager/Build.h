#pragma once
#include <string>
#include <vector>
using namespace std;
#include "VisualStudioInfo.h"
#include "Command.h"

enum class RuntimeConfiguration
{
	Debug = 0,
	Release = 1
};
enum class Configuration
{	
	Debug = 0,
	Release = 1,
	DebugStatic = 2,
	ReleaseStatic = 3
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

constexpr bool IsConfigurationString(std::string s)
{
	return s == "Debug" || s == "Release" || s == "DebugStatic" || s == "ReleaseStatic";
}
constexpr const char* GetConfigurationString(Configuration configuration)
{
	switch (configuration)
	{
	case Configuration::Debug: return "Debug"; break;
	case Configuration::Release: return "Release"; break;
	case Configuration::DebugStatic: return "DebugStatic"; break;
	case Configuration::ReleaseStatic: return "ReleaseStatic"; break;
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
	struct Property
	{
		std::string name;
		std::vector<std::string> values;		
	};

	std::string projectPath;	
	std::vector<Property> properties;	
};

Result BuildProject(VisualStudioInfo vsInfo, BuildSettings settings, std::string additional, std::string& result);

string GetOutputSubDir(Configuration configuration, Platform platform);
string GetIntermediateSubDir(Configuration configuration, Platform platform);
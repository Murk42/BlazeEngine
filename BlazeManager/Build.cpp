#include "Build.h"
#include <iostream>
#include <filesystem>

std::string Replace(std::string s)
{
	std::replace(s.begin(), s.end(), '\\', '/');
	return s;
}

Result BuildProject(VisualStudioInfo vsInfo, BuildSettings settings, std::string additional)
{
	std::string platform =		((int)settings.platform != -1 ? GetPlatformString(settings.platform) : "");
	std::string outputType =	((int)settings.outputType != -1 ? GetBuildOutputTypeString(settings.outputType) : "");
	std::string configuration = ((int)settings.configuration != -1 ? GetConfigurationString(settings.configuration) : "");
	
	bool any =
		(int)settings.configuration != -1 ||
		(int)settings.platform != -1 ||
		(int)settings.outputType != -1 ||
		settings.outputName.size() > 0 ||
		settings.outputDir.size() > 0 ||
		settings.intermediateDir.size() > 0;

	std::string command =		
		"cd " + std::filesystem::path(vsInfo.instalationPath).string() + "\\VC\\Auxiliary\\Build && "
		+ "vcvarsall.bat x64 && "
		+ "cd " + std::filesystem::path(vsInfo.MSBuildPath).parent_path().string() + " && "
		+ "MSBuild.exe"
		+ " \"" + settings.projectPath + "\""
		+ (any ? " -p:" : "")
		+ (platform.size() > 0 ? "Platform=" + platform + ";" : "")
		+ (outputType.size() > 0 ? "ConfigurationType=" + outputType + ";" : "")
		+ (configuration.size() > 0 ? "Configuration=" + configuration + ";" : "")
		+ (settings.outputName.size() > 0 ? "TargetName=\"" + Replace(settings.outputName) + "\";" : "")
		+ (settings.outputDir.size() > 0 ? "OutDir=\"" + Replace(settings.outputDir) + "\";" : "")
		+ (settings.intermediateDir.size() > 0 ? "IntermediateOutputPath=\"" + Replace(settings.intermediateDir) + "\";" : "");

	for (auto& property : settings.properties)
	{
		if (property.values.size() == 0)
			continue;

		command += property.name + "=\"";
		for (auto& value : property.values)
			command += "" + Replace(value) + ";";

		command += "\";";
	}

	if (command.back() == ';')
		command.pop_back();

	command += " " + additional;

	return RunCommand(command);	
}

string GetOutputSubDir(Configuration configuration, Platform platform)
{
	return (string)"build\\bin\\" + GetPlatformString(platform) + "\\" + GetConfigurationString(configuration) + "\\";
}
string GetIntermediateSubDir(Configuration configuration, Platform platform)
{
	return (string)"build\\temp\\" + GetPlatformString(platform) + "\\" + GetConfigurationString(configuration) + "\\";
}

#include "Build.h"
#include <iostream>
#include <filesystem>
#include "Parsing.h"

std::string Replace(std::string s)
{
	std::replace(s.begin(), s.end(), '\\', '/');
	return s;
}

Result BuildProject(VisualStudioInfo vsInfo, BuildSettings settings, std::string additional, std::string& result)
{		
	std::string command =
		"cd " + std::filesystem::path(vsInfo.instalationPath).string() + "\\VC\\Auxiliary\\Build && "
		+ "vcvarsall.bat x64 && "
		+ "cd " + std::filesystem::path(vsInfo.MSBuildPath).parent_path().string() + " && "
		+ "MSBuild.exe"
		+ " \"" + settings.projectPath + "\"";

	if (settings.properties.size() > 0)
		command += " -p:";

	for (auto& property : settings.properties)
	{
		if (property.values.size() == 0)
			continue;

		command += property.name + "=";

		if (property.values.size() == 1 && SplitSymbols(property.values[0]).size() == 1)		
			command += property.values[0] + ";";		
		else
		{
			command += '"';
			for (auto& value : property.values)
				command += "" + Replace(value) + ";";

			if (command.back() == ';')
				command.pop_back();
			command += "\";";
		}
	}

	if (command.back() == ';')
		command.pop_back();

	command += " " + additional;


	return RunCommand(command, result);	
}

string GetOutputSubDir(Configuration configuration, Platform platform)
{
	return (string)"build\\bin\\" + GetConfigurationString(configuration) + "\\" + GetPlatformString(platform) + "\\";
}
string GetIntermediateSubDir(Configuration configuration, Platform platform)
{
	return (string)"build\\temp\\" + GetConfigurationString(configuration) + "\\" + GetPlatformString(platform) + "\\";
}

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
	std::string configuration = GetConfigurationString(settings.configuration);
	std::string platform = GetPlatformString(settings.platform);
	std::string outputType = GetBuildOutputTypeString(settings.outputType);
	
	std::string command = 
		"cd " + std::filesystem::path(vsInfo.MSBuildPath).parent_path().string() + " && "
		+ "MSBuild.exe"		
		+ " \"" + settings.projectPath + "\""
		+ " -p:Configuration=" + configuration
		+ ";Platform=" + platform
		+ ";ConfigurationType=" + outputType
		+ (settings.outputName.size() > 0 ? ";TargetName=\"" + Replace(settings.outputName) + "\"" : "")
		+ (settings.outputDir.size() > 0 ? ";OutDir=\"" + Replace(settings.outputDir) + "\"" : "")
		+ (settings.intermediateDir.size() > 0 ? ";IntermediateOutputPath=\"" + Replace(settings.intermediateDir) + "\"" : "")
		+ (settings.additionalLibraryDirectories.size() > 0 ? ";UserAdditionalLibraryDirectories=\"" + Replace(settings.additionalLibraryDirectories) + "\"" : "")
		+ (settings.additionalDependencies.size() > 0 ? ";UserAdditionalDependecies=\"" + settings.additionalDependencies + "\"" : "")
		+ " " + additional;	

	return RunCommand(command);	
}

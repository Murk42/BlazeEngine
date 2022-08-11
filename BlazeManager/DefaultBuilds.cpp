#include "DefaultBuilds.h"
#include "Parsing.h"
#include <iostream>
#include <filesystem>
#include <fstream>

extern VisualStudioInfo vsInfo;
extern string blazeDir;
extern string runtimeDir;

Result LoadClientLibraryInfo(const string& path, bool log, ClientLibraryInfo& info)
{
	std::ifstream file{ path };

	int lineNum = 0;
	std::string line;
	while (!file.eof())
	{
		getline(file, line);

		std::vector<string> symbols = SplitSymbols(line);

		if (symbols.size() > 0)
		{
			if (symbols[0] == "path")
			{
				if (symbols.size() > 1)
				{
					string path;
					if (!StripQuotes(symbols[1], path))
						return Result("Invalid library.txt file syntax: " + symbols[1]);

					info.additionalLibraryDirectories.emplace_back(path);
				}
				else
					return Result("Invalid library file syntax at line " + to_string(lineNum) + ", no additional library path provided");
			}
			else if (symbols[0] == "lib")
			{
				if (symbols.size() > 1)
				{
					string name;
					if (!StripQuotes(symbols[1], name))
						return Result("Invalid library.txt file syntax: " + symbols[1]);

					info.additionalDependencies.emplace_back(name);
				}
				else
					return Result("Invalid library file syntax at line " + to_string(lineNum) + ", no library name provided");
			}
			else if (symbols[0] == "dll")
			{
				if (symbols.size() > 1)
				{
					string dllPath;
					if (!StripQuotes(symbols[1], dllPath))
						return Result("Invalid library.txt file syntax: " + symbols[1]);

					info.additionalDynamicLibraries.emplace_back(dllPath);
				}
				else
					return Result("Invalid library file syntax at line " + to_string(lineNum) + ", no dll provided");
			}
			else return Result("Invalid library type");
		}
		++lineNum;
	}

	return Result();
}

bool BuildBlaze(Configuration configuration, Platform platform)
{
	BuildSettings blazeBuildSettings{
		.projectPath = blazeDir + "BlazeEngine.vcxproj",
		.outputDir = blazeDir + GetOutputSubDir(configuration, platform),		
		.configuration = configuration,
		.platform = platform,
	};

	Result blazeBuildResult = BuildProject(vsInfo, blazeBuildSettings, "-p:WarningLevel=0");
	
	if (!blazeBuildResult.sucessfull)
	{
		cout << blazeBuildResult.log;
		return true;
	}
	return false;
}

bool BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings)
{		
	string blazeOutputDir = blazeDir + GetOutputSubDir(configuration, platform);

	BuildSettings buildSettings{
		.projectPath = settings.projectPath,
		.outputDir = settings.outputDir,
		.outputName = "Client",			
		.configuration = configuration,
		.platform = ::platform,					
	};

	if (configuration == Configuration::FinalBuild_Debug || configuration == Configuration::FinalBuild_Release)
	{
		buildSettings.outputType = BuildOutputType::StaticLibrary;
		buildSettings.properties = {
			{ "BlazeManagerAdditianalLibraryDirectories", { blazeOutputDir } },
			{ "BlazeManagerAdditionalDependencies", { "BlazeEngine.lib "}}
		};
	}	
	else
	{
		buildSettings.outputType = BuildOutputType::DynamicLibrary;
	}

	Result buildResult = BuildProject(vsInfo, buildSettings, "-p:WarningLevel=0");

	if (!buildResult.sucessfull)
	{
		cout << buildResult.log;
		return true;
	}
	return false;	
}

bool BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings)
{	
	BuildSettings buildSettings{
			.projectPath = runtimeDir + "BlazeEngineRuntime.vcxproj",
			.outputDir = settings.outputDir,			
			.configuration = configuration,
			.platform = ::platform,			
	};

	if (configuration == Configuration::FinalBuild_Debug || configuration == Configuration::FinalBuild_Release)
	{
		string outputSubDir = GetOutputSubDir(configuration, platform);

		std::vector<std::string> additionalLibraryDirectories = settings.clientLibraryInfo.additionalLibraryDirectories;
		std::vector<std::string> additionalDependencies = settings.clientLibraryInfo.additionalDependencies;

		additionalLibraryDirectories.push_back(settings.clientOutputDir);		
		additionalDependencies.push_back("Client.lib");		

		buildSettings.outputType = BuildOutputType::Application;
		buildSettings.properties = {
			{ "BlazeManagerAdditionalLibraryDirectories", additionalLibraryDirectories },
			{ "BlazeManagerAdditionalDependencies", additionalDependencies },
		};
	}
	else
	{
		buildSettings.outputType = BuildOutputType::DynamicLibrary;
	}

	Result runtimeBuildResult = BuildProject(vsInfo, buildSettings, "-p:WarningLevel=0");

	if (!runtimeBuildResult.sucessfull)
	{
		cout << runtimeBuildResult.log;
		return true;
	}
	return false;
}
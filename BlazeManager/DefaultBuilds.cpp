#include "DefaultBuilds.h"
#include "Parsing.h"
#include <iostream>
#include <filesystem>
#include <fstream>

extern VisualStudioInfo vsInfo;

Result LoadClientLibraryInfo(const string& path, bool log, Configuration configuration, ClientLibraryInfo& info)
{
	std::ifstream file{ path };

	bool started = false;
	int lineNum = 0;
	std::string line;
	while (!file.eof())
	{
		getline(file, line);

		std::vector<string> symbols = SplitSymbols(line);

		if (symbols.size() == 0)
			continue;

		if (IsConfigurationString(symbols[0]))
			if (GetConfigurationString(configuration) != symbols[0])
			{
				if (started)
					return Result();

				continue;
			}
			else
			{
				started = true;
				continue;
			}

		if (!started)
			continue;

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

		++lineNum;
	}

	return Result();
}

Result BuildBlaze(Configuration configuration, Platform platform, EngineBuildSettings settings, std::string& result)
{
	BuildSettings blazeBuildSettings{
		.projectPath = settings.projectPath,
		.outputDir = settings.outputDir,		
		.configuration = configuration,
		.platform = platform,
	};

	return BuildProject(vsInfo, blazeBuildSettings, "-p:WarningLevel=0", result);	
}

Result BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings, std::string& result)
{
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
			{ "BlazeManagerAdditionalLibraryDirectories", { settings.engineLibraryDir } },
			{ "BlazeManagerAdditionalDependencies", { "BlazeEngine.lib "}}
		};
	}	
	else
	{
		buildSettings.outputType = BuildOutputType::DynamicLibrary;
	}

	return BuildProject(vsInfo, buildSettings, "-p:WarningLevel=0", result);
}

Result BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings, std::string& result)
{	
	BuildSettings buildSettings{
			.projectPath = settings.projectPath,
			.outputDir = settings.outputDir,
			.configuration = configuration,
			.platform = ::platform,
	};

	if (configuration == Configuration::FinalBuild_Debug || configuration == Configuration::FinalBuild_Release)
	{
		buildSettings.outputType = BuildOutputType::Application;
		buildSettings.properties = {
			{ "BlazeManagerAdditionalLibraryDirectories", { settings.clientLibraryDir } },
			{ "BlazeManagerAdditionalDependencies", { "Client.lib" } },
		};
	}
	else
	{
		buildSettings.outputType = BuildOutputType::DynamicLibrary;
	}

	return BuildProject(vsInfo, buildSettings, "-p:WarningLevel=0", result);
}
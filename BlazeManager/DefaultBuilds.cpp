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

		if (symbols[0] == "asset")
		{
			if (symbols.size() > 1)
			{
				string path;
				if (!StripQuotes(symbols[1], path))
					return Result("Invalid library.txt file syntax: " + symbols[1]);

				info.assetsPaths.emplace_back(path);
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
		.properties = {			
			{ "OutDir", { settings.outputDir } },
			{ "Platform", { GetPlatformString(platform) } },
			{ "Configuration", { GetConfigurationString(configuration) } },
		}
	};

	return BuildProject(vsInfo, blazeBuildSettings, "", result);	
}

Result BuildClient(Configuration configuration, Platform platform, ClientBuildSettings settings, std::string& result)
{
	BuildSettings buildSettings{
		.projectPath = settings.projectPath,
		.properties = {
			{ "TargetName", { "Client"} },
			{ "OutDir", { settings.outputDir } },
			{ "Platform", { GetPlatformString(platform) } },
			{ "Configuration", { GetConfigurationString(configuration) } },			
		}
	};

	if (configuration == Configuration::DebugStatic || configuration == Configuration::ReleaseStatic)
	{				
		buildSettings.properties.push_back({ "BlazeManagerAdditionalLibraryDirectories", { settings.engineLibraryDir } });
		buildSettings.properties.push_back({ "BlazeManagerAdditionalDependencies", { "BlazeEngine.lib "} });
	}		

	return BuildProject(vsInfo, buildSettings, "", result);
}

Result BuildRuntime(Configuration configuration, Platform platform, RuntimeBuildSettings settings, std::string& result)
{	
	BuildSettings buildSettings{
			.projectPath = settings.projectPath,
			.properties = {
				{ "OutDir", { settings.outputDir } },
				{ "Platform", { GetPlatformString(platform) } },
				{ "Configuration", { GetConfigurationString((Configuration)((int)configuration % 2)) } },
				{ "BlazeManagerAdditionalLibraryDirectories", { settings.clientLibraryDir } },
				{ "BlazeManagerAdditionalDependencies", { "Client.lib" } },
			}
	};
			
	return BuildProject(vsInfo, buildSettings, "", result);
}
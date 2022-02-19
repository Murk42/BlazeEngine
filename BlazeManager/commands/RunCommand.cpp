#include "RunCommand.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../Build.h"
#include "../DefaultBuilds.h"
#include "../Library.h"
#include "../Parsing.h"

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define CHECK(x) { if (x) return true; }

extern VisualStudioInfo vsInfo;
extern string blazeDir;
extern string runtimeDir;

Result LoadClientLibraries(std::vector<Library>& libraries, const string& path, const string& outputPath)
{
	std::ifstream file{ path };

	vector<string> paths;

	int lineNum = 0;
	std::string line;
	while (!file.eof())
	{
		getline(file, line);

		vector<string> symbols = SplitSymbols(line);
		
		if (symbols.size() > 0)
		{
			if (symbols[0] == "dll")
			{
				if (symbols.size() > 1)
				{
					string libraryPath;
					if (!StripQuotes(symbols[1], libraryPath))					
						return Result("Invalid library path syntax: " + symbols[1]);

					paths.emplace_back(libraryPath);
				}
				else
					return Result("Invalid library file syntax at line " + to_string(lineNum) + ", no library path provided");
			}
			else			
				return Result("Invalid library type");			
		}		
		++lineNum;
	}

	libraries.resize(paths.size());

	for (int i = 0; i < paths.size(); ++i)
	{
		//string newPath = outputPath + filesystem::path(paths[i]).filename().string();
		//filesystem::copy_file(paths[i], newPath, filesystem::copy_options::overwrite_existing);
		//
		//Result r = libraries[i].LoadLibrary(newPath);
		Result r = libraries[i].LoadLibrary(paths[i]);
		if (!r.sucessfull)
			return r;
	}

	return Result();
}

bool RunCommand(RunCommandOptions options)
{
	Configuration configuration = options.release ? Configuration::Release : Configuration::Debug;
	
	string projectDir = filesystem::path(options.projectPath).parent_path().string() + "\\";
	string projectName = filesystem::path(options.projectPath).stem().string();

	string outputSubDir = GetOutputSubDir(configuration, ::platform);
	string intermediateSubDir = GetIntermediateSubDir(configuration, ::platform);

	string clientLibDir = projectDir + outputSubDir;
	string runtimeLibDir = runtimeDir + outputSubDir;

	string blazeOutputDir = blazeDir + outputSubDir;

	if (!options.dontBuildBlaze)
	{
		cout << "Building blaze...\n";
		CHECK(BuildBlaze(configuration, ::platform));
	}
	
	if (!options.dontBuildClient)
	{
		BuildSettings clientBuildSettings;
		clientBuildSettings.projectPath = options.projectPath;
		clientBuildSettings.platform = ::platform;
		clientBuildSettings.configuration = configuration;
		clientBuildSettings.additionalLibraryDirectories = blazeOutputDir;
		clientBuildSettings.outputDir = clientLibDir;
		clientBuildSettings.outputName = "Client";
		clientBuildSettings.intermediateDir = projectDir + intermediateSubDir;
		clientBuildSettings.outputType = BuildOutputType::DynamicLibrary;

		cout << "Building client project...\n";
		Result clientBuildResult = BuildProject(vsInfo, clientBuildSettings, "-p:WarningLevel=0");

		if (!clientBuildResult.sucessfull)
		{
			cout << clientBuildResult.log;
			return true;
		}
	}

	if (!options.dontBuildRuntime)
	{
		BuildSettings runtimeBuildSettings;
		runtimeBuildSettings.projectPath = runtimeDir + "BlazeEngineRuntime.vcxproj";
		runtimeBuildSettings.platform = ::platform;
		runtimeBuildSettings.configuration = configuration;
		runtimeBuildSettings.additionalLibraryDirectories = clientLibDir + ";" + blazeOutputDir;
		runtimeBuildSettings.additionalDependencies = "Client.lib";
		runtimeBuildSettings.outputDir = runtimeDir + outputSubDir;
		runtimeBuildSettings.intermediateDir = runtimeDir + intermediateSubDir;
		runtimeBuildSettings.outputType = BuildOutputType::DynamicLibrary;

		cout << "Building runtime...\n";
		Result runtimeBuildResult = BuildProject(vsInfo, runtimeBuildSettings, "-p:WarningLevel=0");

		if (!runtimeBuildResult.sucessfull)
		{
			cout << runtimeBuildResult.log;
			return true;
		}
	}

	error_code ec;
	if (!filesystem::exists(projectDir + outputSubDir + "Client.dll", ec))
	{
		std::cout << "Failed to find the client dll.";

		if (ec)	cout << ec.category().name() << ": " << ec.message();

		return true;
	}

	{
		Library libraryDevIL;
		Library libraryILU;
		Library librarySDL2;
		Library libraryBlaze;
		Library libraryClient;
		Library libraryRuntime;
		vector<Library> clientLibraries;

		cout << "Loading libraries...\n";
		Result r;
		string path;

		path = blazeOutputDir + "DevIL.dll";
		r = libraryDevIL.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = blazeOutputDir + "ILU.dll";
		r = libraryILU.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = blazeOutputDir + "SDL2.dll";
		r = librarySDL2.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = blazeOutputDir + "BlazeEngine.dll";
		r = libraryBlaze.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }		

		path = projectDir + "libraries.txt";
		if (filesystem::exists(path))
		{
			r = LoadClientLibraries(clientLibraries, path, clientLibDir);			
			if (!r.sucessfull) { cout << r.log; return true; }
		}

		path = clientLibDir + "Client.dll";
		r = libraryClient.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = runtimeLibDir + "BlazeEngineRuntime.dll";
		r = libraryRuntime.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }


		void(*RUNTIME_START)() = (void(*)())libraryRuntime.GetFunction("RUNTIME_START", r);
		if (!r.sucessfull) { cout << r.log; return true; }

		cout << "Statring:\n";

		string oldPath = std::filesystem::current_path().string();
		
		_chdir(projectDir.c_str());				

		RUNTIME_START();

		_chdir(oldPath.c_str());
	}	

	if (options.stopAfter)
		exit(0);

	return false;
}
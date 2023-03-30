#include "RunCommand.h"

#include <filesystem>
#include <iostream>
using namespace std;

#include "Globals.h"
#include "DefaultBuilds.h"
#include "Library.h"
#include "InstancePath.h"

static Result LoadClientDynamicLibraries(const ClientLibraryInfo& info, std::vector<Library>& libraries, bool log)
{
	libraries.resize(info.additionalDynamicLibraries.size());

	for (int i = 0; i < libraries.size(); ++i)
	{		
		Result r = libraries[i].LoadLibrary(info.additionalDynamicLibraries[i]);

		if (log)
			cout << "<BlazeEngineManager> Loading client library \"" + info.additionalDynamicLibraries[i] + "\"\n";

		if (!r.sucessfull)
			return r;
	}
	
	return Result();
}

Result RunClient(Library& clientLib);

struct RuntimeInfo
{
	bool runtimeLog = true;
	string clientLibraryName;
};

Result RunCommand(RunCommandOptions options)
{	
	RuntimeInfo runtimeInfo;
	runtimeInfo.runtimeLog = options.runtimeLog;
	runtimeInfo.clientLibraryName = "Client.dll";

	Configuration configuration;
	switch (options.configuration)
	{
	case RuntimeConfiguration::Debug: configuration = Configuration::Debug; break;
	case RuntimeConfiguration::Release: configuration = Configuration::Release; break;
	default: throw;
	}

	string projectDir = filesystem::path(options.projectPath).parent_path().string() + "\\";
	string projectName = filesystem::path(options.projectPath).stem().string();
	string outputSubDir = GetOutputSubDir(configuration, platform);
	string engineOutputDir = engineProjectDir + outputSubDir;

	string clientOutputDir = projectDir + outputSubDir;
	string runtimeOutputDir = runtimeProjectDir + outputSubDir;
	
	string buildLog;

	if (options.buildEngine)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building blaze...\n";
		if (Result r = BuildBlaze(configuration, platform, {
			.projectPath = engineProjectDir + "BlazeEngine.vcxproj",
			.outputDir = engineOutputDir
			}, buildLog)) return buildLog + "\n" + std::move(r);

		cout << buildLog;
	}
	if (options.buildClient)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building client project...\n";
		if (Result r = BuildClient(configuration, platform, {
			.projectPath = options.projectPath,
			.outputDir = clientOutputDir,
			.engineLibraryDir = engineOutputDir
			}, buildLog)) return buildLog + "\n" + std::move(r);

		cout << buildLog;
	}		
	if (options.buildRuntime)
	{		
		if (options.managerLog) cout << "<BlazeEngineManager> Building runtime...\n";
		if (Result r = BuildRuntime(configuration, platform, {
			.projectPath = runtimeProjectDir + "BlazeEngineRuntime.vcxproj",
			.outputDir = runtimeOutputDir,
			.clientLibraryDir = clientOutputDir,
			}, buildLog)) return buildLog + "\n" + std::move(r);

		cout << buildLog;
	}
	
	error_code ec;
	{
		string path = projectDir + outputSubDir + projectName + ".dll";
		bool exists = filesystem::exists(path, ec);
		if (ec) return Result("Failed to see if client library exists: \"" + path + "\"\n" + (string)ec.category().name() + ": " + ec.message() + "\n");

		if (!exists) return Result("The client library doesn't exists at the given location: \"" + path + "\"\n");		
	}

	if (options.managerLog)
		cout << "<BlazeEngineManager> Loading libraries...\n";

	//filesystem::create_directory(projectDir + "assets");
	//filesystem::copy(engineOutputDir + "assets\\default", projectDir + "assets\\default", filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive, ec);
	//
	//if (ec) return Result("Failed to copy default assets\n" + (string)ec.category().name() + ": " + ec.message() + "\n");
			
	vector<Library> engineLibraries(engineDependencies[(Configuration)options.configuration].size());
	
	for (int i = 0; i < engineLibraries.size(); ++i)
	{ 
		string path = engineOutputDir + engineDependencies[(Configuration)options.configuration][i];
		if (Result r = engineLibraries[i].LoadLibrary(path))
			return string("Failed to load engine dependency: \"" + path + "\"\n") + std::move(r);
	}
			

	{
		Library engineLibrary;
		void(*InitializeBlaze)();
		void(*TerminateBlaze)();

		{
			string path;

			if (options.buildEngine || engineDLLPath.empty())
				path = engineOutputDir + "BlazeEngine.dll";
			else
				path = engineDLLPath;

			if (options.managerLog)  cout << "<BlazeEngineManager> Loading engine library: \"" + path + "\"\n";

			if (Result r = engineLibrary.LoadLibrary(path))
				return string("Failed to load engine library: \"" + path + "\"\n") + std::move(r);
			
			if (Result r = engineLibrary.GetFunction("InitializeBlaze", InitializeBlaze)) return string("Failed to load engine function \"InitializeBlaze\"\n") + std::move(r);
			if (Result r = engineLibrary.GetFunction("TerminateBlaze", TerminateBlaze)) return string("Failed to load engine function \"TerminateBlaze\"\n") + std::move(r);
		}



		{
			vector<Library> clientLibraries;
			{
				ClientLibraryInfo clientLibraryInfo;
				string path = projectDir + "libraries.txt";

				bool exists = filesystem::exists(path);

				if (ec) return Result("Failed check if file exists: \"" + path + "\"\n" + (string)ec.category().name() + ": " + ec.message() + "\n");

				if (exists)
				{
					if (Result r = LoadClientLibraryInfo(path, options.managerLog, (Configuration)options.configuration, clientLibraryInfo)) return r;					
					if (Result r = LoadClientDynamicLibraries(clientLibraryInfo, clientLibraries, options.managerLog)) return r;					
				}
			}

			{
				InstancePath instancePath{ projectDir };				
				InitializeBlaze();				
				Library libraryClient;

				{
					string path = clientOutputDir + projectName + ".dll";
					if (options.managerLog) cout << "<BlazeEngineManager> Loading client library: \"" + path + "\"\n";
					if (Result r = libraryClient.LoadLibrary(path)) return string("Failed to load client library: \"" + path + "\"\n") + std::move(r);
				}

				{
					//Library libraryRuntime;
					//if (options.buildRuntime || runtimeDLLPath.empty())
					//	path = runtimeOutputDir + "BlazeEngineRuntime.dll";
					//else
					//	path = runtimePathDLL;
					//
					//r = libraryRuntime.LoadLibrary(path);
					//if (!r.sucessfull) { cout << r.log; return true; }

					{
						//void(*RUNTIME_START)(RuntimeInfo) = (void(*)(RuntimeInfo))libraryRuntime.GetFunction("RUNTIME_START", r);
						//if (!r.sucessfull) { cout << r.log; return true; }

						if (options.managerLog) cout << "<BlazeEngineManager> Statring runtime\n\n";


						if (Result r = RunClient(libraryClient))
							return r;
						//RUNTIME_START(runtimeInfo);

					}
				}

				TerminateBlaze();				
			}
		}

	}
	return Result();
}

Result RunClient(Library& clientLibrary)
{	
	void(*Setup)();
	if (Result r = clientLibrary.GetFunction("Setup", Setup)) return string("Failed to load client function \"Setup\"\n") + std::move(r);

	Setup();

	return Result();
}
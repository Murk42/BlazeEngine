#ifdef _WIN32
#include <Windows.h>
#undef LoadLibrary
#endif

#include "RunCommand.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Build.h"
#include "DefaultBuilds.h"
#include "Library.h"
#include "Parsing.h"

#include "../BlazeEngineRuntime/RuntimeInfo.h"

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <chrono>

#define CHECK(x) { if (x) return true; }

extern VisualStudioInfo vsInfo;
extern string blazeDir;
extern string runtimeDir;

#ifdef _WIN32
static string GuiOpenFile()
{
	OPENFILENAMEA ofn = { 0 };
	char Buffer[300];
	fill(Buffer, Buffer + 300, '\0');

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = Buffer;
	ofn.nMaxFile = 300;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrFilter = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	GetOpenFileNameA(&ofn);

	return string(Buffer, 300);
}
#endif

static Result LoadClientDybamicLibraries(const ClientLibraryInfo& info, std::vector<Library>& libraries, bool log)
{
	libraries.resize(info.additionalDynamicLibraries.size());

	for (int i = 0; i < libraries.size(); ++i)
	{
		//string newPath = outputPath + filesystem::path(paths[i]).filename().string();
		//filesystem::copy_file(paths[i], newPath, filesystem::copy_options::overwrite_existing);
		//
		//Result r = libraries[i].LoadLibrary(newPath
		Result r = libraries[i].LoadLibrary(info.additionalDynamicLibraries[i]);

		if (log)
			cout << "<BlazeEngineManager> Loading client library \"" + info.additionalDynamicLibraries[i] + "\"\n";

		if (!r.sucessfull)
			return r;
	}
	
	return Result();
}

double MeasureTime(chrono::high_resolution_clock::time_point& time_point)
{

	auto now = chrono::high_resolution_clock::now();		
	double duration = chrono::duration<double>(now - time_point).count();
	time_point = now;
	return duration;
}

bool RunCommand(RunCommandOptions options)
{
	auto all_timePoint = chrono::high_resolution_clock::now();
	auto other_timePoint = chrono::high_resolution_clock::now();
	RuntimeInfo runtimeInfo;	

#ifdef _WIN32
	if (options.guiOpenFile)	
		options.projectPath = GuiOpenFile();
#endif	

	Configuration configuration = options.release ? Configuration::Release : Configuration::Debug;	

	string projectDir = filesystem::path(options.projectPath).parent_path().string() + "\\";
	string outputSubDir = GetOutputSubDir(configuration, platform);
	string blazeOutputDir = blazeDir + outputSubDir;

	string clientOutputDir = projectDir + outputSubDir;
	string runtimeOutputDir = runtimeDir + outputSubDir;			
	
	if (!options.dontBuildBlaze)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building blaze...\n";
		CHECK(BuildBlaze(configuration, platform));
	}
	if (!options.dontBuildClient)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building client project...\n";		
		CHECK(BuildClient(configuration, platform, { options.projectPath, clientOutputDir }));
	}	
	if (!options.dontBuildRuntime)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building runtime...\n";
		CHECK(BuildRuntime(configuration, platform, { 
			.clientOutputDir = clientOutputDir,
			.outputDir = runtimeOutputDir, 
			}))
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

		if (options.managerLog)
			cout << "<BlazeEngineManager> Loading libraries...\n";

		Result r;
		string path;

		runtimeInfo.timings.other += MeasureTime(other_timePoint);

		path = blazeOutputDir + "DevIL.dll";
		r = libraryDevIL.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = blazeOutputDir + "ILU.dll";
		r = libraryILU.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		path = blazeOutputDir + "SDL2.dll";
		r = librarySDL2.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		runtimeInfo.timings.loadingBlazeLibraries += MeasureTime(other_timePoint);

		path = blazeOutputDir + "BlazeEngine.dll";
		r = libraryBlaze.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }		

		runtimeInfo.timings.loadingBlaze += MeasureTime(other_timePoint);

		ClientLibraryInfo clientLibraryInfo;

		path = projectDir + "libraries.txt";
		if (filesystem::exists(path))
		{
			r = LoadClientLibraryInfo(path, options.managerLog, clientLibraryInfo);
			if (!r.sucessfull) { cout << r.log; return true; }
			r = LoadClientDybamicLibraries(clientLibraryInfo, clientLibraries, options.managerLog);
			if (!r.sucessfull) { cout << r.log; return true; }
		}

		runtimeInfo.timings.loadingClientLibraries += MeasureTime(other_timePoint);

		path = clientOutputDir + "Client.dll";
		r = libraryClient.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		runtimeInfo.timings.loadingClient += MeasureTime(other_timePoint);

		path = runtimeOutputDir + "BlazeEngineRuntime.dll";
		r = libraryRuntime.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		runtimeInfo.timings.loadingRuntime += MeasureTime(other_timePoint);
		
		void(*RUNTIME_START)(RuntimeInfo) = (void(*)(RuntimeInfo))libraryRuntime.GetFunction("RUNTIME_START", r);
		if (!r.sucessfull) { cout << r.log; return true; }

		if (options.managerLog)
			cout << "<BlazeEngineManager> Statring runtime\n\n";

		string oldPath = std::filesystem::current_path().string();
		
		_chdir(projectDir.c_str());				
		
		runtimeInfo.runtimeLog = options.runtimeLog;

		runtimeInfo.timings.other += MeasureTime(other_timePoint);
		runtimeInfo.timings.all += MeasureTime(all_timePoint);
		RUNTIME_START(runtimeInfo);

		_chdir(oldPath.c_str());
	}		

	return false;
}
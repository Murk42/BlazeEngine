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
extern string engineProjectDir;
extern string enginePathDLL;
extern string runtimeProjectDir;
extern string runtimePathDLL;

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
	runtimeInfo.runtimeLog = options.runtimeLog;

#ifdef _WIN32
	if (options.projectPath.empty())	
		options.projectPath = GuiOpenFile();
#endif	

	Configuration configuration;
	switch (options.configuration)
	{
	case RunCommandConfiguration::Debug: configuration = Configuration::Debug; break;
	case RunCommandConfiguration::Release: configuration = Configuration::Release; break;
	default: cout << "Invalid configuration\n"; return true;
	}

	string projectDir = filesystem::path(options.projectPath).parent_path().string() + "\\";
	string outputSubDir = GetOutputSubDir(configuration, platform);
	string engineOutputDir = engineProjectDir + outputSubDir;

	string clientOutputDir = projectDir + outputSubDir;
	string runtimeOutputDir = runtimeProjectDir + outputSubDir;
	
	if (options.buildBlaze)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building blaze...\n";
		CHECK(BuildBlaze(configuration, platform, {
			.projectPath = engineProjectDir + "BlazeEngine.vcxproj",
			.outputDir = engineOutputDir
			}));
	}
	if (options.buildClient)
	{
		if (options.managerLog) cout << "<BlazeEngineManager> Building client project...\n";
		CHECK(BuildClient(configuration, platform, { 
			.projectPath = options.projectPath, 
			.outputDir = clientOutputDir,
			.enginePathLIB = "" 
			}));
	}		
	if (options.buildRuntime)
	{		
		if (options.managerLog) cout << "<BlazeEngineManager> Building runtime...\n";
		CHECK(BuildRuntime(configuration, platform, { 
			.projectPath = runtimeProjectDir + "BlazeEngineRuntime.vcxproj",
			.outputDir = runtimeOutputDir, 
			.clientOutputDir = clientOutputDir,
			}))
	}
	
	error_code ec;
	if (!filesystem::exists(projectDir + outputSubDir + "Client.dll", ec))
	{
		std::cout << "Failed to find the client dll.";
		if (ec)	cout << ec.category().name() << ": " << ec.message();
		return true;
	}

	if (options.managerLog)
		cout << "<BlazeEngineManager> Loading libraries...\n";

	filesystem::create_directory(projectDir + "assets");
	filesystem::copy(engineOutputDir + "assets\\default", projectDir + "assets\\default", filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive, ec);
	if (ec)
	{
		std::cout << "Failed to copy default assets.";
		if (ec)	cout << ec.category().name() << ": " << ec.message();
		return true;
	}

	Result r;
	string path;

	{
		Library libraryDevIL;
		path = engineOutputDir + "DevIL.dll";
		r = libraryDevIL.LoadLibrary(path);
		if (!r.sucessfull) { cout << r.log; return true; }

		{
			Library libraryILU;
			path = engineOutputDir + "ILU.dll";
			r = libraryILU.LoadLibrary(path);
			if (!r.sucessfull) { cout << r.log; return true; }

			{
				Library librarySDL2;
				path = engineOutputDir + "SDL2.dll";
				r = librarySDL2.LoadLibrary(path);
				if (!r.sucessfull) { cout << r.log; return true; }

				{
					Library libraryBlaze;

					if (options.buildBlaze || enginePathDLL.empty())
						path = engineOutputDir + "BlazeEngine.dll";
					else
						path = enginePathDLL;

					r = libraryBlaze.LoadLibrary(path);
					if (!r.sucessfull) { cout << r.log; return true; }

					Result _r;
					void(*InitializeBlaze)() = (void(*)())libraryBlaze.GetFunction("InitializeBlaze", _r);
					if (!r.sucessfull) { cout << r.log; return true; }
					void(*TerminateBlaze)() = (void(*)())libraryBlaze.GetFunction("TerminateBlaze", _r);
					if (!r.sucessfull) { cout << r.log; return true; }

					InitializeBlaze();

					{
						vector<Library> clientLibraries;
						ClientLibraryInfo clientLibraryInfo;

						path = projectDir + "libraries.txt";
						if (filesystem::exists(path))
						{
							r = LoadClientLibraryInfo(path, options.managerLog, clientLibraryInfo);
							if (!r.sucessfull) { cout << r.log; return true; }
							r = LoadClientDybamicLibraries(clientLibraryInfo, clientLibraries, options.managerLog);
							if (!r.sucessfull) { cout << r.log; return true; }
						}

						{
							Library libraryClient;
							path = clientOutputDir + "Client.dll";
							r = libraryClient.LoadLibrary(path);
							if (!r.sucessfull) { cout << r.log; return true; }

							{
								Library libraryRuntime;
								if (options.buildRuntime || runtimePathDLL.empty())
									path = runtimeOutputDir + "BlazeEngineRuntime.dll";
								else
									path = runtimePathDLL;

								r = libraryRuntime.LoadLibrary(path);
								if (!r.sucessfull) { cout << r.log; return true; }

								{
									void(*RUNTIME_START)(RuntimeInfo) = (void(*)(RuntimeInfo))libraryRuntime.GetFunction("RUNTIME_START", r);
									if (!r.sucessfull) { cout << r.log; return true; }

									if (options.managerLog) cout << "<BlazeEngineManager> Statring runtime\n\n";

									string oldPath = std::filesystem::current_path().string();

									_chdir(projectDir.c_str());

									RUNTIME_START(runtimeInfo);

									_chdir(oldPath.c_str());
								}
							}
						}
					}

					TerminateBlaze();
				}
			}
		}
	}

	return false;
}
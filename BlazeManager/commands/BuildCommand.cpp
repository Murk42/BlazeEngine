#ifdef _WIN32
#include <Windows.h>
#endif

#include "BuildCommand.h"
#include <filesystem>
#include <iostream>

#include "../DefaultBuilds.h"


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

bool BuildCommand(BuildCommandOptions options)
{	
#ifdef _WIN32
	if (options.guiOpenFile)
	{
		options.projectPath = GuiOpenFile();
	}
#endif
	string projectDir = filesystem::path(options.projectPath).parent_path().string() + "\\";
	string projectName = filesystem::path(options.projectPath).stem().string();

	string outputSubDir = GetOutputSubDir(options.configuration, options.platform);
	string intermediateSubDir = GetIntermediateSubDir(options.configuration, options.platform);

	string blazeOutputDir = blazeDir + outputSubDir;

	CHECK(BuildBlaze(options.configuration, options.platform));

	BuildSettings clientBuildSettings;
	clientBuildSettings.projectPath = options.projectPath;
	clientBuildSettings.platform = options.platform;
	clientBuildSettings.configuration = options.configuration;
	clientBuildSettings.additionalLibraryDirectories = blazeOutputDir;
	clientBuildSettings.outputDir = projectDir + outputSubDir;
	clientBuildSettings.intermediateDir = projectDir + intermediateSubDir;

	Result clientBuildResult = BuildProject(vsInfo, clientBuildSettings, "-p:WarningLevel=0");

	if (!clientBuildResult.sucessfull)
	{
		cout << clientBuildResult.log;
		return true;
	}

	BuildSettings runtimeBuildSettings;
	runtimeBuildSettings.projectPath = runtimeDir + "BlazeEngineRuntime.vcxproj";
	runtimeBuildSettings.platform = options.platform;
	runtimeBuildSettings.configuration = options.configuration;
	runtimeBuildSettings.outputType = BuildOutputType::Application;
	runtimeBuildSettings.additionalLibraryDirectories = clientBuildSettings.outputDir + ";" + blazeOutputDir;
	runtimeBuildSettings.additionalDependencies = "Client.lib";
	runtimeBuildSettings.outputDir = runtimeDir + outputSubDir;
	runtimeBuildSettings.intermediateDir = runtimeDir + intermediateSubDir;

	Result runtimeBuildResult = BuildProject(vsInfo, runtimeBuildSettings, "-p:WarningLevel=0");

	if (!runtimeBuildResult.sucessfull)
	{
		cout << runtimeBuildResult.log;
		return true;
	}

	error_code ec;
	filesystem::copy_file(blazeOutputDir + "DevIL.dll", options.outputDir + "DevIL.dll", filesystem::copy_options::overwrite_existing, ec);
	if (ec) { cout << ec.category().name() << ": " << ec.message(); return true; }
	filesystem::copy_file(blazeOutputDir + "ILU.dll", options.outputDir + "ILU.dll", filesystem::copy_options::overwrite_existing, ec);
	if (ec) { cout << ec.category().name() << ": " << ec.message(); return true; }
	filesystem::copy_file(blazeOutputDir + "SDL2.dll", options.outputDir + "SDL2.dll", filesystem::copy_options::overwrite_existing, ec);
	if (ec) { cout << ec.category().name() << ": " << ec.message(); return true; }
	filesystem::copy(projectDir + "assets", options.outputDir + "assets", filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive, ec);
	if (ec) { cout << ec.category().name() << ": " << ec.message(); return true; }
	filesystem::copy_file(runtimeBuildSettings.outputDir + "BlazeEngineRuntime.exe", options.outputDir + options.outputName, filesystem::copy_options::overwrite_existing, ec);
	if (ec) { cout << ec.category().name() << ": " << ec.message(); return true; }
}
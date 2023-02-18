#include "Globals.h"

VisualStudioInfo vsInfo;

string engineProjectDir = "C:\\Programming\\Projects\\BlazeEngine\\BlazeEngine\\";
string engineDLLPath;
string engineLIBPath;
unordered_map<Configuration, vector<string>> engineDependencies{
	{ Configuration::Debug,					{ "SDL2d.dll", "DevIL.dll", "ILU.dll", "freetyped.dll"} },
	{ Configuration::Release,				{ "SDL2.dll", "DevIL.dll", "ILU.dll", "freetype.dll"} },
	{ Configuration::FinalBuild_Debug,		{ "SDL2d.dll", "DevIL.dll", "ILU.dll", "freetyped.dll"} },
	{ Configuration::FinalBuild_Release,	{ "SDL2.dll", "DevIL.dll", "ILU.dll", "freetype.dll"} },
};

string runtimeProjectDir = "C:\\Programming\\Projects\\BlazeEngine\\BlazeEngineRuntime\\";
string runtimeDLLPath;
string runtimeLIBPath;
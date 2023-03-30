#include "Globals.h"

	VisualStudioInfo vsInfo;

string engineProjectDir = "D:\\Programming\\Projects\\BlazeEngine\\BlazeEngine\\";
string engineDLLPath;
string engineLIBPath;
unordered_map<Configuration, vector<string>> engineDependencies{
	{ Configuration::Debug,					{ "SDL2d.dll", "DevIL.dll", "ILU.dll", "freetyped.dll", "glew32d.dll" }},
	{ Configuration::Release,				{ "SDL2.dll", "DevIL.dll", "ILU.dll", "freetype.dll", "glew32.dll" } },
	{ Configuration::DebugStatic,			{ "SDL2d.dll", "DevIL.dll", "ILU.dll", "freetyped.dll", "glew32d.dll" } },
	{ Configuration::ReleaseStatic,			{ "SDL2.dll", "DevIL.dll", "ILU.dll", "freetype.dll", "glew32.dll" } },
};

string runtimeProjectDir = "D:\\Programming\\Projects\\BlazeEngine\\BlazeEngineRuntime\\";
string runtimeDLLPath;
string runtimeLIBPath;
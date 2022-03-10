#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include "BlazeEngine/Application/Application.h"
using namespace std;

#if defined(FINAL_BUILD)
extern "C" void InitializeBlaze();
extern "C" void TerminateBlaze();
extern "C" bool GetApplicationClassInfo(Blaze::BlazeApplicationClassInfo*&);
extern "C" void InvalidateApplicationClassInfo();

#if !defined(FINAL_BUILD_EMPTY)
extern "C" void Setup();
#else
void Setup()
{

}
#endif

#else
#include "VisualStudioInfo.h"
#include <iostream>
#include <thread>
#include <filesystem>
#include "Command.h"
#include "Environment.h"
#include "Library.h"
#include "Build.h"
#include "Parsing.h"

LibraryView blazeLibrary;
LibraryView clientLibrary;

void(*InitializeBlaze)();
void(*TerminateBlaze)();
//bool(*GetApplicationClassInfo)(Blaze::BlazeApplicationClassInfo*&);
//void(*InvalidateApplicationClassInfo)();
void(*Setup)();

#define LOAD_FUNC(x, y, z) x = (decltype(x))y.GetFunction(#x, z); if (!z.sucessfull) { result.log += "Failed to load function \"" #x "\"\n"; return result; }

Result LoadBlazeFunctions()
{
	Result result;

	LOAD_FUNC(InitializeBlaze, blazeLibrary, result);	
	LOAD_FUNC(TerminateBlaze, blazeLibrary, result);	
	//LOAD_FUNC(GetApplicationClassInfo, blazeLibrary, result);	
	//LOAD_FUNC(InvalidateApplicationClassInfo, blazeLibrary, result);	
	return Result();
}
Result LoadClientFunctions()
{
	Result result;

	LOAD_FUNC(Setup, clientLibrary, result);	

	return Result();
}
 
bool ResolveResult(Result r)
{
	if (!r.sucessfull)
	{
		cout << r.log;
		return true;
	}
	return false;
}
#define RESOLVE(x) if (ResolveResult(x)) exit(1);
#endif 

#if !defined(FINAL_BUILD)	
extern "C" __declspec(dllexport) int RUNTIME_START()
{	
	RESOLVE(blazeLibrary.Set("BlazeEngine.dll"));
	RESOLVE(clientLibrary.Set("Client.dll"));

	blazeLibrary.Set("BlazeEngine.dll");	
	RESOLVE(LoadBlazeFunctions());
	
	clientLibrary.Set("Client.dll");	
	RESOLVE(LoadClientFunctions());	
#else
int main()
{	
#endif
//#if defined(FINAL_BUILD_DEBUG)
	cout << "BlazeEngineRuntime: Started\n";	
//#endif

	cout << "BlazeEngineRuntime: Initializing Blaze\n";
	InitializeBlaze();
	cout << "BlazeEngineRuntime: Blaze initialized\n";

	cout << "BlazeEngineRuntime: Starting client\n";
	Setup();

	cout << "BlazeEngineRuntime: Terminating Blaze\n";
	TerminateBlaze();
	cout << "BlazeEngineRuntime: Blaze terminated\n";

	return 0;
}
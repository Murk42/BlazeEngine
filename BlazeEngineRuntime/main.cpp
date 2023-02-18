#if defined(FINAL_BUILD) && defined(_WIN32)
#include <Windows.h>
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
using namespace std;

#if defined(FINAL_BUILD)
#define RUNTIME_API
extern "C" void Setup();
extern "C" void InitializeBlaze();
extern "C" void TerminateBlaze();
#else
#define RUNTIME_API extern "C" __declspec(dllexport)
#include "Result.h"
#include "Library.h"

void(*Setup)();

LibraryView blazeLibrary;
LibraryView clientLibrary;

struct RuntimeInfo
{
	bool runtimeLog;
	string clientLibraryName;
};

#define LOAD_FUNC(x, y, z) x = (decltype(x))y.GetFunction(#x, z); if (!z.sucessfull) { result.log += "Failed to load function \"" #x "\"\n"; return result; }
#define RESOLVE(x) if (ResolveResult(x)) exit(1);

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


#endif

#if !defined(FINAL_BUILD)
extern "C" __declspec(dllexport) int RUNTIME_START(RuntimeInfo runtimeInfo)
{
#elif !defined(_WIN32)
int main(int argc, char* argv[])
{	
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#endif

#ifdef FINAL_BUILD
	InitializeBlaze();
#else
	RESOLVE(blazeLibrary.Set("BlazeEngine.dll"));
	RESOLVE(clientLibrary.Set(runtimeInfo.clientLibraryName));
		
	RESOLVE(LoadClientFunctions());

	if (runtimeInfo.runtimeLog)
	{
		cout << "BlazeEngineRuntime: Started\n";

		cout << "BlazeEngineRuntime: Initializing Blaze\n";
	}
#endif
	
	Setup();

#ifdef FINAL_BUILD
	TerminateBlaze();
#endif

	return 0;
}
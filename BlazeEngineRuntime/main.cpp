#if defined(FINAL_BUILD) && defined(_WIN32)
#include <Windows.h>
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
#include <chrono>
#include <fstream>
#include <mutex>
using namespace std;

#include "BlazeEngine/Core/Startup.h"
#include "RuntimeInfo.h"


#if defined(FINAL_BUILD)
#define RUNTIME_API
extern "C" Blaze::Startup::BlazeInitInfo InitializeBlaze();
extern "C" void TerminateBlaze();
extern "C" void SetStartupInfo(Blaze::Startup::StartupInfo);
extern "C" void Setup();
#else
#define RUNTIME_API extern "C" __declspec(dllexport)
#include "Result.h"
#include "Library.h"


struct AllocationData
{
	size_t size;
	void* ptr;
};
struct MemoryReport
{
	std::mutex m;
	size_t count = 0;
	AllocationData data[1024];
};

Blaze::Startup::BlazeInitInfo(*InitializeBlaze)();
void(*TerminateBlaze)();
void(*SetStartupInfo)(Blaze::Startup::StartupInfo);
void(*Setup)();
void(*SetMemoryReport)(MemoryReport*);

LibraryView blazeLibrary;
LibraryView clientLibrary;

#define LOAD_FUNC(x, y, z) x = (decltype(x))y.GetFunction(#x, z); if (!z.sucessfull) { result.log += "Failed to load function \"" #x "\"\n"; return result; }
#define RESOLVE(x) if (ResolveResult(x)) exit(1);

Result LoadBlazeFunctions()
{
	Result result;

	LOAD_FUNC(InitializeBlaze, blazeLibrary, result);	
	LOAD_FUNC(TerminateBlaze, blazeLibrary, result);	
	LOAD_FUNC(SetStartupInfo, blazeLibrary, result);
	LOAD_FUNC(SetMemoryReport, blazeLibrary, result);

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


#endif

double MeasureTime(chrono::high_resolution_clock::time_point& time_point)
{
	auto now = chrono::high_resolution_clock::now();
	double duration = chrono::duration<double>(now - time_point).count();
	time_point = now;
	return duration;
}

void SaveMemoryReport(MemoryReport* report)
{
	std::ofstream file("memory.txt");

	for (int i = 0; i < report->count; ++i)	
		file << report->data[i].ptr << " " << report->data[i].size << "\n";

	file.close();
}

#if !defined(FINAL_BUILD)
extern "C" __declspec(dllexport) int RUNTIME_START(RuntimeInfo runtimeInfo)
{
#else
#if !defined(_WIN32)
int main()
{	
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#endif

	RuntimeInfo runtimeInfo;
	runtimeInfo.timings.all = 0;
	runtimeInfo.timings.loadingBlaze = 0;
	runtimeInfo.timings.loadingBlazeLibraries = 0;
	runtimeInfo.timings.loadingClient = 0;
	runtimeInfo.timings.loadingClientLibraries = 0;
	runtimeInfo.timings.loadingRuntime = 0;
	runtimeInfo.timings.other = 0;
#endif
	auto startTimePoint = chrono::high_resolution_clock::now();

#if !defined(FINAL_BUILD)
	RESOLVE(blazeLibrary.Set("BlazeEngine.dll"));
	RESOLVE(clientLibrary.Set("Client.dll"));
	
	RESOLVE(LoadBlazeFunctions());
	RESOLVE(LoadClientFunctions());

	MemoryReport* memoryReport = new MemoryReport;
	SetMemoryReport(memoryReport);

	if (runtimeInfo.runtimeLog)
	{
		cout << "BlazeEngineRuntime: Started\n";

		cout << "BlazeEngineRuntime: Initializing Blaze\n";
	}
#endif
	auto blazeInitInfo = InitializeBlaze();
#if !defined(FINAL_BUILD)	
	if (runtimeInfo.runtimeLog)
	{
		cout << "BlazeEngineRuntime: Blaze initialized\n";

		cout << "BlazeEngineRuntime: Starting client\n";
	}
#endif

	Blaze::Startup::StartupInfo startupInfo;	
	startupInfo.blazeLoadTime =
		runtimeInfo.timings.loadingBlazeLibraries +
		runtimeInfo.timings.loadingBlaze +
		runtimeInfo.timings.loadingRuntime +
		runtimeInfo.timings.other;
	startupInfo.clientLibrariesLoadTime =
		runtimeInfo.timings.loadingClientLibraries;
	startupInfo.clientLoadTime =
		runtimeInfo.timings.loadingClient;
	
	startupInfo.initializationTime =
		runtimeInfo.timings.all +
		MeasureTime(startTimePoint);

	startupInfo.blazeInitInfo = blazeInitInfo;	

	SetStartupInfo(startupInfo);
	Setup();

#if !defined(FINAL_BUILD)	
	if (runtimeInfo.runtimeLog)
	cout << "BlazeEngineRuntime: Terminating Blaze\n";
#endif
	TerminateBlaze();
#if !defined(FINAL_BUILD)	
	if (runtimeInfo.runtimeLog)
	cout << "BlazeEngineRuntime: Blaze terminated\n";
#endif

	SaveMemoryReport(memoryReport);
	delete memoryReport;

	return 0;
}
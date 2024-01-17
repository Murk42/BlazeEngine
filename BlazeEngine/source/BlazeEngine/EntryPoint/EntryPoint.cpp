#include "pch.h"
#include "BlazeEngine/EntryPoint/EntryPoint.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include "SDL2/SDL_events.h"

#ifdef BLAZE_STATIC
extern void Setup();
extern void AddLoggerOutputFiles();
#else
static void(*Setup)();
static void(*AddLoggerOutputFiles)();
#endif

namespace Blaze
{
	TimingResult InitializeMemory(); 
	void TerminateMemory();

	TimingResult InitializeConsole();
	void TerminateConsole();

	TimingResult InitializeBlazeEngine();
	void TerminateBlazeEngine();

	TimingResult InitializeLibraries();
	void TerminateLibraries();

	TimingResult InitializeInput();
	void TerminateInput(); 

	TimingResult InitializeGraphics();
	void TerminateGraphics();
}

void ReportSubTiming(Blaze::TimingResult& result, Blaze::uintMem indent)
{
	Blaze::String indentString;
	indentString.Resize(indent * 4, ' ');
	Blaze::Debug::Logger::LogInfo("Blaze Engine", indentString + result.name + " - " + Blaze::StringParsing::Convert(result.time, Blaze::StringParsing::FloatStringFormat::Fixed) + "s");

	for (auto& subResult : result.nodes)
		ReportSubTiming(subResult.value, indent + 1);
}

void ReportTiming(Blaze::Timing& timing)
{	
	auto result = timing.GetTimingResult();
	Blaze::Debug::Logger::LogInfo("Blaze Engine", result.name + " initialization took " + Blaze::StringParsing::Convert(result.time, Blaze::StringParsing::FloatStringFormat::Fixed) + "s");

	for (auto& subResult : result.nodes)
		ReportSubTiming(subResult.value, 1);	
}

extern "C" void InitializeBlaze()
{
	Blaze::Timing timing { "Blaze engine"};

	AddLoggerOutputFiles();
		
	timing.AddNode(Blaze::InitializeMemory());
	timing.AddNode(Blaze::InitializeBlazeEngine());
	timing.AddNode(Blaze::InitializeConsole());
	timing.AddNode(Blaze::InitializeLibraries());
	timing.AddNode(Blaze::InitializeInput());

	//ReportTiming(timing);

	Blaze::globalData->blazeInitTimings = timing.GetTimingResult();
}
extern "C" void TerminateBlaze()
{	
	Blaze::TerminateInput();
	Blaze::TerminateLibraries();
	Blaze::TerminateConsole();
	Blaze::TerminateBlazeEngine();
	Blaze::TerminateMemory();
}

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

int RunSetupOnThread()
{
	struct ThreadExitReporter
	{
		~ThreadExitReporter() 
		{
			SDL_Event event;
			SDL_memset(&event, 0, sizeof(event));
			event.type = Blaze::globalData->clientThreadExitEventIdentifier;
			SDL_PushEvent(&event);
		}
	};

	ThreadExitReporter threadExitReporter;
	Setup();

	return 0;
}

static void EmptyFunc()
{

}

static Blaze::StringView GetSDLError()
{
	const char* ptr = SDL_GetError();
	Blaze::uintMem len = strlen(ptr);
	return Blaze::StringView(ptr, len);
}


BLAZE_API int main(int argc, char* argv[])
{
#ifndef BLAZE_STATIC
	HMODULE applicationModule = GetModuleHandleA(NULL);
	 
	if (applicationModule == nullptr)
		return 1;

	Setup = (void(*)())GetProcAddress(applicationModule, "Setup");
	AddLoggerOutputFiles = (void(*)())GetProcAddress(applicationModule, "AddLoggerOutputFiles");

	if (AddLoggerOutputFiles == nullptr)
		AddLoggerOutputFiles = EmptyFunc;

	if (Setup == nullptr)
		return 1;
#endif

	InitializeBlaze();
		
	Blaze::Thread clientThread;
	clientThread.Run(RunSetupOnThread);			
	
	while (!Blaze::globalData->clientThreadExited.test())
	{
		SDL_Event event;
		if (SDL_WaitEvent(&event) == 0)			
			Blaze::Debug::Logger::LogError("SDL", "SDL_WaitEvent failed, SDL_GetError returned: \"" + GetSDLError() + "\"");

		Blaze::globalData->CheckForMainThreadTask();
	}

	while (clientThread.IsRunning());

	TerminateBlaze();
}
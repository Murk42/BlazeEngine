#include "pch.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include "SDL2/SDL_events.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef BLAZE_STATIC
extern void Setup();
extern void AddLoggerOutputFiles();
#else
static void(*Setup)();
static void(*AddLoggerOutputFiles)();
#endif

namespace Blaze
{
	void AddLoggerOutputFiles();

	TimingResult InitializeConsole();
	void TerminateConsole();	

	TimingResult InitializeLibraries();
	void TerminateLibraries();

	TimingResult InitializeInput();
	void TerminateInput();

	static void ReportSubTiming(TimingResult& result, uintMem indent)
	{
		String indentString;
		indentString.Resize(indent * 4, ' ');
		Debug::Logger::LogInfo("Blaze Engine", indentString + result.name + " - " + StringParsing::Convert(result.time.ToSeconds(), StringParsing::FloatStringFormat::Fixed) + "s");

		for (auto& subResult : result.nodes)
			ReportSubTiming(subResult.value, indent + 1);
	}

	static void ReportTiming(Timing& timing)
	{
		auto result = timing.GetTimingResult();
		Debug::Logger::LogInfo("Blaze Engine", result.name + " initialization took " + StringParsing::Convert(result.time.ToSeconds(), StringParsing::FloatStringFormat::Fixed) + "s");

		for (auto& subResult : result.nodes)
			ReportSubTiming(subResult.value, 1);
	}

	static void AddLoggerOutputFiles()
	{
	}

	static void InitializeBlaze()
	{
		Timing timing{ "Blaze engine" };

		AddLoggerOutputFiles();
			
		timing.AddNode(InitializeConsole());
		timing.AddNode(InitializeLibraries());
		timing.AddNode(InitializeInput());

		//ReportTiming(timing);

		blazeEngineContext.blazeInitTimings = timing.GetTimingResult();
	}
	static void TerminateBlaze()
	{
		TerminateInput();
		TerminateLibraries();
		TerminateConsole();				
	}

	static unsigned RunSetupOnThread()
	{
		struct ThreadExitReporter
		{
			~ThreadExitReporter()
			{
				SDL_Event event;
				SDL_memset(&event, 0, sizeof(event));
				event.type = blazeEngineContext.clientThreadExitEventIdentifier;
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

	static StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	static int BlazeMain()
	{
#ifndef BLAZE_STATIC
#ifdef BLAZE_PLATFORM_WINDOWS
		HMODULE applicationModule = GetModuleHandleA(NULL);

		if (applicationModule == nullptr)
			return 1;

		Setup = (void(*)())GetProcAddress(applicationModule, "Setup");
		AddLoggerOutputFiles = (void(*)())GetProcAddress(applicationModule, "AddLoggerOutputFiles");

		if (AddLoggerOutputFiles == nullptr)
			AddLoggerOutputFiles = EmptyFunc;

		if (Setup == nullptr)
			return 1;
#else
#error unsuported platform
#endif
#endif		

		Thread clientThread;
		clientThread.Run(RunSetupOnThread);

		while (!blazeEngineContext.clientThreadExited.test())
		{
			SDL_Event event;
			if (SDL_WaitEvent(&event) == 0)
				Debug::Logger::LogError("SDL", "SDL_WaitEvent failed, SDL_GetError returned: \"" + GetSDLError() + "\"");			

			//Must be run here because the SDLEventWatcher can be run on a another thread
			blazeEngineContext.ExecuteMainThreadTask();
		}

		while (clientThread.IsRunning());

		return 0;
	}
}

#ifdef BLAZE_PLATFORM_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	Blaze::InitializeBlaze();

	Blaze::blazeEngineContext.nCmdShow = nCmdShow;

	int ret = Blaze::BlazeMain();

	Blaze::TerminateBlaze();

	return ret;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	Blaze::InitializeBlaze();

	Blaze::blazeEngineContext.nCmdShow = nCmdShow;

	int ret = Blaze::BlazeMain();

	Blaze::TerminateBlaze();

	return ret;
}
#endif

int main(int argc, char* argv[])
{
	Blaze::InitializeBlaze();

	int ret = Blaze::BlazeMain();

	Blaze::TerminateBlaze();

	return ret;
}
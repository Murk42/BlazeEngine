#include "pch.h"
#include "BlazeEngineCore/Threading/Thread.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef BLAZE_STATIC
extern void Setup();
extern void AddLoggerOutputFiles();

static void AddLoggerOutputFiles()
{
}
#else
static void(*Setup)();
static void(*AddLoggerOutputFiles)();
#endif

namespace Blaze
{

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
		BLAZE_ENGINE_CORE_INFO("{} {} - {:.4}s", indentString, result.name, result.time.ToSeconds());

		for (auto& subResult : result.nodes)
			ReportSubTiming(subResult.value, indent + 1);
	}

	static void ReportTiming(Timing& timing)
	{
		auto result = timing.GetTimingResult();
		BLAZE_ENGINE_CORE_INFO("{} initialization took {:.4}s", result.name, result.time.ToSeconds());

		for (auto& subResult : result.nodes)
			ReportSubTiming(subResult.value, 1);
	}


	static void InitializeBlaze()
	{
		Timing timing{ "Blaze engine" };

#ifdef BLAZE_STATIC
		AddLoggerOutputFiles();
#else
		if (AddLoggerOutputFiles != nullptr)
			AddLoggerOutputFiles();
#endif
			
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
				Blaze::PostSDLClientThreadExitEvent();				
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

		if (blazeEngineContext.createClientThread)
		{
			Thread clientThread;
			clientThread.Run(&RunSetupOnThread);

			while (!blazeEngineContext.clientThreadExited.test())
			{
				SDL_Event event;
				if (SDL_WaitEvent(&event) == 0)
					Debug::Logger::LogError("SDL", "SDL_WaitEvent failed, SDL returned error: \"" + GetSDLError() + "\"");

				blazeEngineContext.ExecuteMainThreadTask();
			}

			if (!clientThread.WaitToFinish(5.0f))
				BLAZE_ENGINE_CORE_WARNING("Client thread didnt finish 5 seconds after notifying the main thread that it is exiting");
		}
		else		
			Setup();	
				
		SDL_Event event;
		while (SDL_PollEvent(&event));
			
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
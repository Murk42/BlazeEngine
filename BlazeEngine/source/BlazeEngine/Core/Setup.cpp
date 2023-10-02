#include "pch.h"
#include "BlazeEngine/Core/Setup.h"
#include "BlazeEngine/Threading/Thread.h"

namespace Blaze
{
	TimingResult InitializeMemory(); 
	void TerminateMemory();

	TimingResult InitializeConsole();
	void TerminateConsole();

	TimingResult InitializeEngineData();
	void TerminateEngineData();

	TimingResult InitializeLibraries();
	void TerminateLibraries();

	TimingResult InitializeInput();
	void TerminateInput(); 

	TimingResult InitializeGraphics();
	void TerminateGraphics();
}

namespace Blaze::Input
{
	void HandleEvents();
}

extern "C" void InitializeBlaze()
{
	Blaze::Timing timing { "Blaze engine"};

	timing.AddNode(Blaze::InitializeMemory());
	timing.AddNode(Blaze::InitializeConsole());
	timing.AddNode(Blaze::InitializeEngineData());
	timing.AddNode(Blaze::InitializeLibraries());
	timing.AddNode(Blaze::InitializeInput());
	//timing.AddNode(Blaze::InitializeGraphics());

	engineData->blazeInitTimings = timing.GetTimingResult();
}
extern "C" void TerminateBlaze()
{
	//Blaze::TerminateGraphics();
	Blaze::TerminateInput();
	Blaze::TerminateConsole();
	Blaze::TerminateLibraries();
	Blaze::TerminateEngineData();
	Blaze::TerminateMemory();
}

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif


#ifdef BLAZE_STATIC
extern void Setup();
#else
static void(*Setup)();
#endif

int RunSetupOnThread()
{
	Setup();

	return 0;
}


BLAZE_API int main(int argc, char* argv[])
{
#ifndef BLAZE_STATIC
	HMODULE applicationModule = GetModuleHandleA(NULL);
	 
	if (applicationModule == nullptr)
		return 1;

	Setup = (void(*)())GetProcAddress(applicationModule, "Setup");

	if (Setup == nullptr)
		return 1;
#endif

	InitializeBlaze();
		
	Blaze::Thread clientThread;
	clientThread.Run(RunSetupOnThread);		

	while (!clientThread.IsRunning());
	while (clientThread.IsRunning())
	{
		Blaze::Input::HandleEvents();
	}

	TerminateBlaze();
}
#include "BlazeEngine/Core/Startup.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Utilities/Time.h"
#include "source/BlazeEngine/Internal/Libraries.h"

namespace Blaze
{
	void InitializeMemory();
	void TerminateMemory();
	Startup::ConsoleInitInfo InitializeConsole();
	void TerminateConsole();
	Startup::InputInitInfo InitializeInput();
	void TerminateInput();	
	Startup::GraphicsInitInfo InitializeGraphics();
	void TerminateGraphics();
	void InitializeEngineData();
	void TerminateEngineData();
}


extern "C" BLAZE_API void InitializeBlaze()
{
	Blaze::TimePoint timePoint = Blaze::TimePoint::GetWorldTime();

	Blaze::InitializeMemory();
	Blaze::InitializeEngineData();

	Blaze::Startup::BlazeInitInfo& initInfo = engineData->initInfo;
	initInfo.libraryInitInfo = Blaze::InitializeLibraries();
	initInfo.consoleInitInfo = Blaze::InitializeConsole();
	initInfo.inputInitInfo = Blaze::InitializeInput();	
	initInfo.graphicsInitInfo = Blaze::InitializeGraphics();
	initInfo.initTime = timePoint - Blaze::TimePoint::GetWorldTime();

	initInfo.initTime = Blaze::TimePoint::GetWorldTime() - timePoint;	

	engineData->finishedInit = true;
}
extern "C" BLAZE_API void TerminateBlaze()
{
	Blaze::TerminateGraphics();	
	Blaze::TerminateInput();
	Blaze::TerminateConsole();
	Blaze::TerminateLibraries();
	Blaze::TerminateEngineData();
	Blaze::TerminateMemory();
}
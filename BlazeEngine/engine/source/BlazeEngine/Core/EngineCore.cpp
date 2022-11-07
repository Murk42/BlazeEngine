#include "BlazeEngine/Core/Startup.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Core/MemoryManager.h"
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
	Startup::RendererInitInfo InitializeRenderer();
	void TerminateRenderer();
	Startup::GraphicsInitInfo InitializeGraphics();
	void TerminateGraphics();
}

extern "C" BLAZE_API Blaze::Startup::BlazeInitInfo InitializeBlaze()
{
	Blaze::TimePoint timePoint = Blaze::TimePoint::GetWorldTime();
	Blaze::Startup::BlazeInitInfo initInfo;

	engineData = new EngineData;

	Blaze::InitializeMemory();
	initInfo.libraryInitInfo = Blaze::InitializeLibraries();
	initInfo.consoleInitInfo = Blaze::InitializeConsole();
	initInfo.inputInitInfo = Blaze::InitializeInput();
	initInfo.rendererInitInfo = Blaze::InitializeRenderer();
	initInfo.graphicsInitInfo = Blaze::InitializeGraphics();
	initInfo.initTime = timePoint - Blaze::TimePoint::GetWorldTime();

	initInfo.initTime = Blaze::TimePoint::GetWorldTime() - timePoint;
	return initInfo;
}
extern "C" BLAZE_API void TerminateBlaze()
{
	Blaze::TerminateGraphics();
	Blaze::TerminateRenderer();
	Blaze::TerminateInput();
	Blaze::TerminateConsole();
	Blaze::TerminateLibraries();
	Blaze::TerminateMemory();

	delete engineData;
}
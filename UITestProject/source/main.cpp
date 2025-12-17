#include "pch.h"
#include "MainAppRuntimeThread.h"
#include "UIDebugLayer.h"


CLIENT_API void Setup()
{
	Debug::Logger::AddOutputFile("log.txt");

	ResourceManager resourceManager;

	App app;
	
	auto mainRuntimeThreadCreationData = GetMainRuntimeThreadCreationData(app);
	app.RegisterRuntimeThread(std::move(mainRuntimeThreadCreationData));

	app.Run();
}
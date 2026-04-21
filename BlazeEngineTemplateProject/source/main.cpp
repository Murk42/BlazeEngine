#include "pch.h"
#include "MainAppRuntimeThread.h"
#include "Layer.h"

bool PreInitialization()
{
	Debug::Logger::AddOutputFile("log.txt");
	return true;
}

int Main()
{

	ResourceManager resourceManager;

	App app;

	app.RegisterRuntimeThread(AppRuntimeThreadCreationData::New<MainThread>().RegisterLayer<Layer>());

	while (app.Update())
	{
		Input::Update();
	}

	return 0; 
}
#include "pch.h"
#include "Threads/MainThread.h"
#include "Layers/MainLayer.h"
 
bool PreInitialization()
{
	Debug::Logger::AddOutputFile("log.txt");
	return true;
}
 
int Main() 
{	
	App app;	
	app.RegisterRuntimeThread(AppRuntimeThreadCreationData::New<AppThread>().RegisterLayer<MainLayer>()
		//.RegisterLayer<UIDebugLayer>()
	);
	app.Start();
	return 0;
}  
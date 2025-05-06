#include "pch.h" 

#include "RenderingSystem.h"
#include "CameraController.h"  
 
CLIENT_API void AddLoggerOutputFiles()
{
	Debug::Logger::AddOutputFile("Log.txt");
}          

CLIENT_API void Setup()
{			
	RenderingSystem renderSystem;

	Window& window = renderSystem.GetWindow();
	window.ShowWindow(true);
	window.Raise();	

	bool exit = false;
	LambdaEventHandler<Input::Events::WindowCloseEvent> keyDown{ [&](auto event) {
		exit = true;
	} };	
	window.closeEventDispatcher.AddHandler(keyDown);	

	CameraController cameraController;
	cameraController.SetWindow(&window);

	window.resizedEventDispatcher.Call({ &window, window.GetSize() });

	while (!exit)
	{		
		Input::Update();
				
		renderSystem.SetViewMatrix(cameraController.GetViewMatrix());
		renderSystem.Render();
	}
}
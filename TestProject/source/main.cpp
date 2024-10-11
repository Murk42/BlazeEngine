#include "pch.h" 

#include "Graphics/OpenGL/RenderSystem_OpenGL.h"

#include "MainScreen.h"

CLIENT_API void AddLoggerOutputFiles()
{
	Debug::Logger::AddOutputFile("Log.txt");
}

CLIENT_API void Setup()
{		
	Resource::ResourceManager resourceManager;
	RenderSystem_OpenGL renderSystem{ resourceManager };

	{
		Font font = Font::LoadDefault();
		font.dataMap.Insert<Graphics::OpenGL::FontAtlasesData_OpenGL>("FontAtlasesData_OpenGL", &font, Array<uint>({ 12, 16, 20, 24, 32, 256 }), CharacterSet::ASCIICharacterSet());
		resourceManager.LoadFont("default", std::move(font));
	}


	WindowBase& window = renderSystem.GetWindow();
	window.ShowWindow(true);
	window.Raise();

	MainScreen mainScreen{ resourceManager };

	mainScreen.SetTransform(UI::NodeTransform({
			.pos = { 0.0f, 0.0f},
			.parentPivot = { 0.0f, 0.0f},
			.pivot = { 0.0f, 0.0f},
			.size = (Vec2f)window.GetSize(),
			.scale = 1.0f,
			.rotation = 0.0f,
		}));

	renderSystem.SetScreen(&mainScreen);

	
	UI::InputManager inputManager;
	inputManager.SetScreen(&mainScreen);
	inputManager.SetWindow(&window);

	bool exit = false;
	LambdaEventHandler<Input::Events::WindowCloseEvent> keyPressed{ [&](auto event) {
		exit = true;
	} };
	LambdaEventHandler<Input::Events::WindowResizedEvent> windowResized{ [&](Input::Events::WindowResizedEvent event) {
		mainScreen.SetTransform(UI::NodeTransform({
			.pos = {0.0f, 0.0f},
			.parentPivot = { 0.0f, 0.0f},
			.pivot = { 0.0f, 0.0f},
			.size = (Vec2f)event.size,
			.scale = 1.0f,
			.rotation = 0.0f,
		}));
	} };

	window.closeEventDispatcher.AddHandler(keyPressed);
	window.resizedEventDispatcher.AddHandler(windowResized);

	while (!exit)
	{		
		Input::Update();
		
		mainScreen.Update();
		mainScreen.CalculateAllTransforms();
		renderSystem.Frame();
	}
}
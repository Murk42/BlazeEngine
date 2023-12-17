#include "pch.h" 

//#if defined(GRAPHICS_OPENGL)
#include "Graphics/OpenGL/RenderSystem_OpenGL.h"
//using RenderSystem = RenderSystem_OpenGL;
//#elif defined(GRAPHICS_VULKAN)
//#include "Graphics/Vulkan/RenderSystem_Vulkan.h"
//using RenderSystem = RenderSystem_Vulkan;
//#endif

#include "MainScreen.h"


CLIENT_API void Setup()
{		
	Debug::Logger::AddOutputFile("Log.txt");

	Font font{ "assets/fonts/Consola.ttf" };			

	Resource::ResourceManager resourceManager;
	RenderSystem_OpenGL renderSystem{ resourceManager };

	WindowBase& window = renderSystem.GetWindow();
	window.ShowWindow(true);

	MainScreen mainScreen{ resourceManager };

	mainScreen.SetTransform(UI2::NodeTransform({
			.pos = { 0.0f, 0.0f},
			.parentPivot = { 0.0f, 0.0f},
			.pivot = { 0.0f, 0.0f},
			.size = (Vec2f)window.GetSize(),
			.scale = { 1.0f, 1.0f },
			.rotation = 0.0f,
		}));

	renderSystem.SetActiveScreen(&mainScreen);

	bool exit = false;
	LambdaEventHandler<Input::Events::WindowCloseEvent> keyPressed{ [&](auto event) {
		exit = true;
	} };
	LambdaEventHandler<Input::Events::WindowResizedEvent> windowResized{ [&](Input::Events::WindowResizedEvent event) {
		mainScreen.SetTransform(UI2::NodeTransform({
			.pos = {0.0f, 0.0f},
			.parentPivot = { 0.0f, 0.0f},
			.pivot = { 0.0f, 0.0f},
			.size = (Vec2f)event.size,
			.scale = { 1.0f, 1.0f },
			.rotation = 0.0f,
		}));
	} };

	window.closeEventDispatcher.AddHandler(keyPressed);
	window.resizedEventDispatcher.AddHandler(windowResized);

	while (!exit)
	{
		Input::Update();
		mainScreen.CalculateAllTransforms();
		renderSystem.Frame();
	}
}
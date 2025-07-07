#include "pch.h"
#include "MainScreen.h"

static ResourceRef<FontFace> LoadFontFace(ResourceManager& resourceManager)
{
	Path path = "assets/fonts/Rekalgera-Regular.otf";
	Font font(path);

	for (uintMem i = 0; i < font.GetFaceCount(); ++i)
	{
		FontFace fontFace(path, i);

		if (!fontFace.IsBold() && !fontFace.IsItalic())
		{
			return resourceManager.LoadResource<FontFace>("default", std::move(fontFace));
			break;
		}
	}	

	return ResourceRef<FontFace>();
}

CLIENT_API void Setup()
{		
	Debug::Logger::AddOutputFile("log.txt");

	ResourceManager resourceManager;
	auto marlboroFontFace = LoadFontFace(resourceManager);

	//resourceManager.LoadResource<Font>("Malboro", resourceManager.LoadResource<File>("assets/fonts/Marlboro.ttf", "assets/fonts/Marlboro.ttf", FileAccessPermission::Read));
	//resourceManager.LoadResource<Font>("Vogue", resourceManager.LoadResource<File>("assets/fonts/Vogue.ttf", "assets/fonts/Vogue.ttf", FileAccessPermission::Read));
	//resourceManager.LoadResource<Font>("African", resourceManager.LoadResource<File>("assets/fonts/african.ttf", "assets/fonts/african.ttf", FileAccessPermission::Read));
		

	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ Graphics::OpenGL::GraphicsContextProperties_OpenGL{						
		} };
	Graphics::OpenGL::RenderWindow_OpenGL renderWindow{ graphicsContext, WindowCreateOptions{			
		.hidden = true,
		.title = "UI test"
	}};
	graphicsContext.SetActiveRenderWindow(renderWindow);
	auto& window = renderWindow.GetWindow();
	bool windowClosed = false;

	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::OpenGL::ColoredCharacterRenderer_OpenGL coloredCharacterRenderer{ graphicsContext };
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer{ graphicsContext };

	Graphics::OpenGL::UIRenderPipeline_OpenGL UIRenderPipeline{ texturedRectRenderer, coloredCharacterRenderer, panelRenderer };			

	MainScreen mainScreen(resourceManager);
	mainScreen.SetWindow(&window);

	UIRenderPipeline.SetScreen(&mainScreen);	

	LambdaEventHandler<Window::WindowResizedEvent> windowResizedEventHandler{
		[&](const Window::WindowResizedEvent& event) {			
		
			graphicsContext.Flush();
			graphicsContext.SetRenderArea(Vec2i(), event.size);			
		} };
	LambdaEventHandler<Window::WindowCloseEvent> windowCloseEventHandler{
		[&](const Window::WindowCloseEvent& event) {
			windowClosed = true;
		} };
	window.windowResizedEventDispatcher.AddHandler(windowResizedEventHandler);
	window.windowCloseEventDispatcher.AddHandler(windowCloseEventHandler);
	windowResizedEventHandler.OnEvent({ .window = window, .size = window.GetSize() });
	window.SetHiddenFlag(false);

	while (true)
	{
		resourceManager.HandleResourceLoadedCallbacks();
		Input::Update();

		if (Keyboard::GetFrameKeyState(Keyboard::Key::F4).pressed && Keyboard::GetFrameKeyState(Keyboard::Key::LALT).pressed)
			break;

		if (windowClosed)
			break;
		
		graphicsContext.ClearTarget();
		
		for (auto& node : mainScreen.GetTree())
			node.CleanFinalTransform();
		UIRenderPipeline.Render(renderWindow.GetSize());
		
		window.SwapBuffers();
	}
}
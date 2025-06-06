#include "pch.h"
#include "MainScreen.h"

CLIENT_API void Setup()
{		
	ResourceManager resourceManager;

	Debug::Logger::AddOutputFile("log.txt");
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

	fonts.Resize(4);
	fonts[0] = Font::LoadDefault();
	fonts[1] = Font("assets/fonts/Marlboro.ttf");
	fonts[2] = Font("assets/fonts/Vogue.ttf");
	fonts[3] = Font("assets/fonts/african.ttf");
	fontStyles.Insert("default", UI::FontStyle(fonts[0], 20, CharacterSet::ASCIICharacterSet()));
	fontStyles.Insert("large", UI::FontStyle(fonts[1], 32, CharacterSet::ASCIICharacterSet()));
	fontStyles.Insert("small", UI::FontStyle(fonts[0], 12, CharacterSet::ASCIICharacterSet()));

	MainScreen mainScreen(resourceManager);
	mainScreen.SetWindow(&window);

	UIRenderPipeline.SetScreen(&mainScreen);	

	LambdaEventHandler<Window::WindowResizedEvent> windowResizedEventHandler{
		[&](const Window::WindowResizedEvent& event) {			
		
			graphicsContext.Flush();
			graphicsContext.SetRenderArea(Vec2i(), event.size);

			auto transform = mainScreen.GetTransform();
			transform.size = (Vec2f)event.size;
			mainScreen.SetTransform(transform);						
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
		

		UIRenderPipeline.Render(renderWindow.GetSize());
		
		window.SwapBuffers();
	}

	fonts.Clear();
}
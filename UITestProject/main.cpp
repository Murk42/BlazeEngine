#include "pch.h"
#include "MainScreen.h"

Array<Font> fonts;

CLIENT_API void Setup()
{			 	
	Debug::Logger::AddOutputFile("log.txt");
	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ Graphics::OpenGL::GraphicsContextProperties_OpenGL{						
		} };
	Graphics::OpenGL::RenderWindow_OpenGL renderWindow{ graphicsContext, WindowCreateOptions{			
		.hidden = true,
		.title = "UI test", 		
	}};
	graphicsContext.SetActiveRenderWindow(renderWindow);
	auto& window = renderWindow.GetWindow();
	bool windowClosed = false;

	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::OpenGL::ColoredCharacterRenderer_OpenGL coloredCharacterRenderer{ graphicsContext };
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer{ graphicsContext };

	Graphics::OpenGL::UIRenderPipeline_OpenGL UIRenderPipeline{ texturedRectRenderer, coloredCharacterRenderer, panelRenderer };
	UI::InputManager InputManager;

	fonts.Resize(4);
	fonts[0] = Font::LoadDefault();
	fonts[1] = Font("assets/fonts/Marlboro.ttf");
	fonts[2] = Font("assets/fonts/Vogue.ttf");
	fonts[3] = Font("assets/fonts/african.ttf");
	for (auto& font : fonts)
		Graphics::FontAtlasesData::AddToFont(font, Array<uint>({ 12, 32, 64 }), CharacterSet::ASCIICharacterSet());	

	MainScreen mainScreen;
	mainScreen.SetWindow(&window);

	UIRenderPipeline.SetScreen(&mainScreen);
	InputManager.SetScreen(&mainScreen);

	Console::WriteLine(StringParsing::Convert(window.GetDisplayIndex()));

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
		Input::Update();		

		if (windowClosed)
			break;		
		
		graphicsContext.ClearTarget();	
		

		UIRenderPipeline.Render(renderWindow.GetSize());
		
		window.SwapBuffers();		
	}

	fonts.Clear();
}
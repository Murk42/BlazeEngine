#include "pch.h"
#include "MainScreen.h"

Array<Font> fonts;

CLIENT_API void Setup()
{			 	
	Debug::Logger::AddOutputFile("log.txt");
	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ Graphics::OpenGL::GraphicsContextProperties_OpenGL{		
		.majorVersion = 4,
		.minorVersion = 0,
		.profileType = Graphics::OpenGL::ProfileType::Core,
		} };
	Graphics::OpenGL::RenderWindow_OpenGL renderWindow{ graphicsContext, Graphics::OpenGL::WindowSDLCreateOptions_OpenGL { } };
	auto& window = renderWindow.GetWindowSDL();
	bool windowClosed = false;

	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer{ graphicsContext };

	Graphics::OpenGL::UIRenderPipeline_OpenGL UIRenderPipeline{ texturedRectRenderer, panelRenderer };
	UI::UIInputManager UIInputManager;

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
	UIInputManager.AddScreen(&mainScreen);

	LambdaEventHandler<Input::Events::WindowResizedEvent> windowResizedEventHandler{
		[&](const Input::Events::WindowResizedEvent& event) {			
		
			graphicsContext.Flush();
			graphicsContext.SetRenderArea(Vec2i(), event.size);

			auto transform = mainScreen.GetTransform();
			transform.size = (Vec2f)event.size;
			mainScreen.SetTransform(transform);						
		} };
	LambdaEventHandler<Input::Events::WindowCloseEvent> windowCloseEventHandler{
		[&](const Input::Events::WindowCloseEvent& event) {
			windowClosed = true;
		} };
	window.resizedEventDispatcher.AddHandler(windowResizedEventHandler);
	window.closeEventDispatcher.AddHandler(windowCloseEventHandler);
	window.SetSize({ 1000, 1000 });
	windowResizedEventHandler.OnEvent({ .window = &window, .size = window.GetSize() });
	window.ShowWindow(true);	

	while (true)
	{
		Input::Update();		

		if (windowClosed)
			break;
		
		graphicsContext.ClearTarget();	
		

		UIRenderPipeline.Render(renderWindow.GetSize());

		//Graphics::OpenGL::UIRenderPipelineDebugData debug;
		//UIRenderPipeline.GetDebugData(debug);
		
		window.SwapBuffers();		
	}

	fonts.Clear();
}
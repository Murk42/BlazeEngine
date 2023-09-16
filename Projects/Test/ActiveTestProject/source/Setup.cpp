//#include "Graphics3/API/API.h"

//#define GRAPHICS_VULKAN
#include "Graphics3/API/API.h"
#include "QuadRenderer/QuadRenderer.h"
#include "EmptyRenderer/EmptyRenderer.h" 
#include "TextRenderer/TextRenderer.h"
               
class Demo
{
public:           
	bool Run(Graphics3::ImplementationInfo* info)
	{		
		Graphics3::GraphicsContext graphicsContext = Graphics3::CreateGraphicsContext(info);
		Graphics3::RenderWindow mainWindow = Graphics3::CreateRenderWindow(graphicsContext, "Main", Vec2i(640, 480));
		List<Graphics3::RenderWindow> sideWindows;		
		 
		Font font{ "assets/default/fonts/Consola.ttf" };
		FontResolution font32{ font, 32 , CharacterSet::ASCIICharacterSet() };		

		bool loop = true;
		bool returnCode = false;
		 
		LambdaEventHandler<Input::Events::WindowCloseEvent> windowCloseEventHandler { [&](auto)
		{
				loop = false;   
		} };
		Input::GetInputEventSystem().windowCloseDispatcher.AddHandler(windowCloseEventHandler);
		LambdaEventHandler<Input::Events::KeyPressed> keyPressedEventHandler { [&](Input::Events::KeyPressed event)
			{
				if (event.key == Key::Escape)
					loop = false;
				if (event.key == Key::R)
				{
					loop = false;
					returnCode = true;
				}
				if (event.key == Key::KeypadPlus)
					sideWindows.AddBack(Graphics3::CreateRenderWindow(graphicsContext, "Side", Vec2i(640, 480)));
				else if (event.key == Key::KeypadMinus)
					sideWindows.EraseFirst();
		} };
		Input::GetInputEventSystem().keyPressedDispatcher.AddHandler(keyPressedEventHandler);
		LambdaEventHandler<Input::Events::WindowResizedEvent> windowResizedEventHandler { [&](Input::Events::WindowResizedEvent event) {
			//Debug::Logger::LogDebug("Client", "Resizing window titled \"" + event.window->GetTitle() + "\" to " + StringParsing::Convert(event.size.x) + "x" + StringParsing::Convert(event.size.y));
		} };
		Input::GetInputEventSystem().windowResizedDispatcher.AddHandler(windowResizedEventHandler);
		 
		Graphics3::Semaphore framebufferAvailableSemaphore1 = Graphics3::CreateSemaphore(graphicsContext);
		//Graphics3::Semaphore framebufferAvailableSemaphore2 = Graphics3::CreateSemaphore(graphicsContext);
		Graphics3::Semaphore renderFinishedSemaphore1 = Graphics3::CreateSemaphore(graphicsContext);
		//Graphics3::Semaphore renderFinishedSemaphore2 = Graphics3::CreateSemaphore(graphicsContext);

		Graphics3::RenderContext renderContext = Graphics3::CreatePresentRenderContext(graphicsContext);

		Graphics3::QuadRenderer quadRenderer { graphicsContext, renderContext };
		Graphics3::EmptyRenderer emptyRenderer { graphicsContext, renderContext };
		Graphics3::TextRenderer textRenderer { graphicsContext, renderContext };				

		auto render = [&](Graphics3::RenderWindow& window, Graphics3::Semaphore& s1, Graphics3::Semaphore& s2)
		{			                 
			auto& framebuffer = Graphics3::AquireNextFramebuffer(window, s1);			

			if (framebuffer.GetSize() == Vec2i())
				return;
			
			SingleLineTextLayouter textLayouter{ "Marko", font, font32 };
			Graphics3::TextRenderData renderData;						
			renderData.fontResolution = &font32;
			renderData.font = &font;
			renderData.fontHeight = 32;
			renderData.textLayouter = &textLayouter;

			textRenderer.RenderText(renderContext, framebuffer, renderData, { &s1 }, { &s2 });
			
			Graphics3::PresentRenderWindow(renderContext, window, framebuffer, { &s2 });					
		};

		double sum = 0;
		uint count = 0;
		Stopwatch stopwatch;
		while (loop)
		{			
			Input::Update();

			Timing renderStopwatch;
			render(mainWindow, framebufferAvailableSemaphore1, renderFinishedSemaphore1);			
			
			sum += renderStopwatch.GetTimingResult().time;
			++count;

			if (stopwatch.GetTime() >= 1.0)
			{
				Console::WriteLine("Average render time: " + StringParsing::Convert(sum / count));

				stopwatch.Reset();
				sum = 0;
				count = 0;
			}
		}

		graphicsContext.WaitForIdle();

		return returnCode;
	}
};
  
CLIENT_API void Setup() 
{    		
	Debug::Logger::SetOutputFile("logs/logs.txt");

	auto impl1 = Graphics3::RegisterImplementation({ Graphics3::ImplementationAPI::Vulkan, "Vulkan" });
	auto impl2 = Graphics3::RegisterImplementation({ Graphics3::ImplementationAPI::OpenGL, "OpenGL" });
	
	while (true)
	{
		Demo* demo = new Demo();

		bool res = demo->Run(impl1);

		delete demo;

		if (res)
			std::swap(impl1, impl2);
		else
			break;
	}
} 
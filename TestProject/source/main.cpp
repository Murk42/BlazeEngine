#include "pch.h"
#include "Graphics3/API/API.h"
#include "Graphics4/API.h"

CLIENT_API void Setup()
{		
	auto openGLImplementation = Graphics3::RegisterImplementation({ Graphics3::ImplementationAPI::OpenGL, "OpenGL" });

	Graphics3::GraphicsContext gc { openGLImplementation };
	Graphics3::RenderWindow rw { gc, "Marko", Vec2i(INT_MAX), Vec2i(640, 480) };

	Graphics4::RenderPipeline rp{ gc };	
	auto clearRenderStage = rp.AddRenderStage(Graphics4::ClearRenderStage(rw));
	auto textRenderStage = rp.AddRenderStage(Graphics4::TextRenderStage(gc));
	auto presentRenderStage = rp.AddRenderStage(Graphics4::PresentRenderStage(rw));			

	bool exit = false;
	LambdaEventHandler<Input::Events::WindowCloseEvent> keyPressed{ [&](auto event) {		
		exit = true;
	} };
	rw.GetWindow()->closeEventDispatcher.AddHandler(keyPressed);	

	rw.GetWindow()->ShowWindow(true);

	Graphics3::Semaphore ss{ gc };

	while (!exit)
	{
		Input::Update();		
		
		auto& fb = rw.AcquireNextFramebuffer(ss);		
				
		rp.PerformPipeline(fb, {});		
	}
}
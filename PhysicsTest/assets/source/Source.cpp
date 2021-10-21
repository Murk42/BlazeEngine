#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Graphics;
using namespace Core;

class App : public BlazeEngine<App>
{
public:	 
	Window window;

	struct Object
	{
		Vec2f pos;
		Vec2f vel;
		Vec2f force;		
	};
	struct Vertex
	{
		Vec2f pos;
	};

	static constexpr size_t objectCount = 16;
	Object objects[objectCount]; 
	Vertex vertices[objectCount];
	 
	GraphicsBuffer vb;
	VertexArray va;
	Program program;		 
	 
	void Startup() override
	{		 		
		Input::SetEventFunction(WindowClosed);		
		Input::SetEventFunction(WindowResized);		
		Input::SetEventFunction(KeyPressed);
		Input::SetEventFunction(KeyReleased);		
		Input::SetEventFunction(MousePressed);
		Input::SetEventFunction(MouseReleased);

		window.SetWindowed(true, false); 
		window.SetSize(Vec2i(500, 500));  
		window.ShowWindow(true);
		 
		Renderer::SetTarget(window);	
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::EnableProgramPointSize(true);		

		{
			VertexShader vs{ "assets/vertex.vert" };
			FragmentShader fs{ "assets/fragment.frag" };

			program.LinkShaders({ &vs, &fs }); 
			int mvpUniform = program.GetUniformLocation("u_MVP");
			program.SetUniform(mvpUniform, Math::OrthographicMatrix<float>(window.GetSize().x / -2, window.GetSize().x / 2, window.GetSize().y / -2, window.GetSize().y / 2, -1, 1));
			int sizeUniform = program.GetUniformLocation("u_size");
			program.SetUniform(sizeUniform, 25.0f);
		} 
		
		for (uint i = 0; i < objectCount; ++i)
		{
			objects[i].pos.x = Random::Float(-250, 250);
			objects[i].pos.y = Random::Float(-250, 250);

			objects[i].vel.x = Random::Float(-0.5, 0.5);
			objects[i].vel.y = Random::Float(-0.5, 0.5);
		}

		vb.AllocateStaticStorage(BufferView(nullptr, sizeof(vertices)), GraphicsBufferStaticStorageHint::DynamicStorage);
		va.SetVertexAttributeData(0, VertexAttribute(&vb, sizeof(Vertex), 0, VertexAttributeType::Float, 2, false, 0));
	}

	void UpdateMesh()
  	{		
		for (int i = 0; i < objectCount; ++i)
		{
			vertices[i].pos.x = objects[i].pos.x;
			vertices[i].pos.y = objects[i].pos.y; 
		}
		vb.ChangeData(BufferView(vertices, sizeof(vertices)), 0);
	}

	static constexpr float physicsStep = 0.01;
	Stopwatch physicsStopwatch;
	Stopwatch renderStopwatch;
	
	void UpdatePhysics()
	{
		for (uint i = 0; i < objectCount; ++i)
		{						
			float distanceSqr = objects[i].pos.SqrLenght();
			float distance = Math::Sqrt(distanceSqr);
			Vec2f dir = -objects[i].pos / distance * physicsStep;

			objects[i].vel += dir;			
			objects[i].pos += objects[i].vel;
		}
	}
	void RenderScene()
	{		
		Renderer::ClearTarget();

		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&va);		
		Renderer::RenderPointArray(objectCount, 0);
		
		Renderer::UpdateTarget();
	}
	
	void Frame() override
	{						
		if (physicsStopwatch.GetTime() > physicsStep) 
		{
			physicsStopwatch.Reset();
			UpdatePhysics();
		}

		if (renderStopwatch.GetTime() > 1.0f / 60)
		{
			renderStopwatch.Reset(); 
			UpdateMesh();
			RenderScene();
		}
	}  

	void Cleanup() override
	{ 

	}	 
		
	InputEventFunction::KeyPressed KeyPressed = [&](InputEvent::KeyPressed event) {
		Console::Write("KeyPressed. " + GetKeyName(event.key) + "\n");
	}; 
	InputEventFunction::KeyReleased KeyReleased = [&](InputEvent::KeyReleased event) {
		Console::Write("KeyReleased. " + GetKeyName(event.key) + "\n");
	};
	InputEventFunction::MousePressed MousePressed = [&](InputEvent::MousePressed event) {
		Console::Write("MousePressed. " + GetKeyName(event.key) + "\n");
	};
	InputEventFunction::MouseReleased MouseReleased = [&](InputEvent::MouseReleased event) {
		Console::Write("MouseReleased. " + GetKeyName(event.key) + "\n");
	};
	InputEventFunction::WindowClosed WindowClosed = [&](InputEvent::WindowClosed event) {
		Stop();
	};
	InputEventFunction::WindowResized WindowResized = [&](InputEvent::WindowResized event)
	{
		int mvpUniform = program.GetUniformLocation("u_MVP");
		program.SetUniform(mvpUniform, Math::OrthographicMatrix<float>(event.w / -2, event.w / 2, event.h / -2, event.h / 2, -1, 1));
		Renderer::SetViewport(Vec2i(), Vec2i(event.w, event.h));
	};
};
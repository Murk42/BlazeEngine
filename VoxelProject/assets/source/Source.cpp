#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Graphics;
using namespace Core;

#include "World.h"

class App : public BlazeEngine<App>
{
public:	 
	Window window;	

	Font font;
	Charmap charmap;
	Program textProgram;
	TextMaterial textMaterial; 

	struct {
		struct {
			TextMesh mesh;
			Transform2D transform;
		} FPStext;
	} UI;

	Vec2f cameraAngles;
	Transform3D cameraTransform;

	Mat4f VP3D;
	Mat4f proj3D;
	Mat4f VP2D;
	Mat4f VP2Dstats;
	
	Program worldProgram;
	World world;

	void LoadAssets()
	{
		{
			font.Load("assets/default/fonts/VertigoFLF.ttf");
			font.LoadCharmap(charmap, 50);
		} 

		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/normalText/vertex.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/normalText/geometry.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/normalText/fragment.glsl");
			textProgram.LinkShaders({ &vertexShader, &fragmentShader, &geometryShader });
		}

		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/world/shaders/vertex.glsl");			
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/world/shaders/geometry.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/world/shaders/fragment.glsl");
			worldProgram.LinkShaders({ &vertexShader, &fragmentShader, &geometryShader });
		}

		{
			textMaterial.SetProgram(&textProgram);
			textMaterial.color.value = Vec4f(0.3, 0.3, 0.3, 1.0);
			textMaterial.texture.value = &charmap.GetTexture();
		}
		 
		{
			UI.FPStext.mesh.SetCharmap(&charmap);
			UI.FPStext.mesh.SetMaterial(&textMaterial);
			UI.FPStext.transform.pos = Vec2i(10, -10);
			UI.FPStext.transform.parentAlign = Align::TopLeft;
		}

		{			
			world.material.SetProgram(&worldProgram);
		}
	} 
	void Startup() override
	{		
		Input::SetEventFunction<InputEventType::WindowClosed>(WindowClosed);		
		Input::SetEventFunction<InputEventType::WindowResized>(WindowResized);

		window.SetWindowed(true, false);		
		window.ShowWindow(true);	 
		window.SetSize({ 500, 500 }); 
		  
		Renderer::SetTarget(window);		
		Renderer::SetClearColor(ColorRGBA(135, 206, 235, 255));				
		WindowResized(window.GetSize().x, window.GetSize().y, window);

		LoadAssets(); 
		 
		Time::SetDeltaTimeLimit(0.01); 
		world.SetViewDistance(5); 				
	} 

	void UpdateEngine()
	{		
		Time::Update();
		Input::Update();
		Logger::FlushLogs();
	}    
	void UpdateInput() 
	{  
		static constexpr float cameraSpeed = 100.0f;
		static constexpr float cameraRotateSpeed = 3.0f;
		if (Input::GetKeyState(Key::W).state == KeyState::Down)
		{ 
			Vec3f d = cameraTransform.rot * Vec3f(0, 0, 1);
			d.Normalise();
			cameraTransform.pos += Vec3f(d.x, 0, d.z) * cameraSpeed * Time::GetDeltaTime();
		}
		if (Input::GetKeyState(Key::A).state == KeyState::Down)
		{
			Vec3f d = cameraTransform.rot * Vec3f(-1, 0, 0);
			d.Normalise();
			cameraTransform.pos += Vec3f(d.x, 0, d.z) * cameraSpeed * Time::GetDeltaTime();
		}
		if (Input::GetKeyState(Key::S).state == KeyState::Down)
		{
			Vec3f d = cameraTransform.rot * Vec3f(0, 0, -1);
			d.Normalise();
			cameraTransform.pos += Vec3f(d.x, 0, d.z) * cameraSpeed * Time::GetDeltaTime();
		}
		if (Input::GetKeyState(Key::D).state == KeyState::Down)
		{
			Vec3f d = cameraTransform.rot * Vec3f(1, 0, 0);
			d.Normalise();   
			cameraTransform.pos += Vec3f(d.x, 0, d.z) * cameraSpeed * Time::GetDeltaTime();
		}
		if (Input::GetKeyState(Key::Space).state == KeyState::Down)
			cameraTransform.pos += Vec3f(0, 1, 0) * cameraSpeed * Time::GetDeltaTime();
		if (Input::GetKeyState(Key::LShift).state == KeyState::Down)
			cameraTransform.pos += Vec3f(0, -1, 0) * cameraSpeed * Time::GetDeltaTime();

		if (Input::GetKeyState(Key::MouseRight).state == KeyState::Down)
		{
			Vec2i mm = Input::GetMouseMovement();
			cameraAngles.x += mm.y * cameraRotateSpeed * Time::GetDeltaTime();
			cameraAngles.y += mm.x * cameraRotateSpeed * Time::GetDeltaTime();

			cameraTransform.rot = Quatf(AxisAnglef(Vec3f(0, 1, 0), cameraAngles.y)) * Quatf(AxisAnglef(Vec3f(1, 0, 0), cameraAngles.x));
		}

		/*
		if (Input::GetKeyState(Key::MouseLeft).state == KeyState::Pressed)
		{
			Block* block = world.GetBlock(cameraTransform.absolutePos);
			if (block != 0) 
			{
				block->id = BlockID::Full;
				world.UpdateChunkMesh(cameraTransform.absolutePos);
			} 
		}*/
	}
	void UpdateTransforms()
	{
		UI.FPStext.transform.Update();
		cameraTransform.UpdateAsViewTransform();
	}
	void Render()
	{		
		Renderer::ClearTarget();

		world.Render(VP3D);

		UI.FPStext.mesh.Render(VP2D * UI.FPStext.transform.mat);

		Renderer::UpdateTarget(); 
	}
	void Frame() override
	{
		UpdateEngine();
		UpdateInput();
		 
		world.UpdateChunks(cameraTransform.pos);

		UI.FPStext.mesh.SetText("FPS: " + String::Convert(Time::GetFPS()));		
		UI.FPStext.transform.size = UI.FPStext.mesh.GetSize();

		UpdateTransforms();

		VP3D = proj3D * cameraTransform.mat;
		Render();
	}
	  
	void Cleanup() override
	{ 

	}

	void NewLog(const Log& log) override
	{
		BlazeEngineBase::NewLog(log);		
	}
				
	InputEventFunction<InputEventType::WindowClosed> WindowClosed = [&](Window&) {
		Stop();
	};
	InputEventFunction<InputEventType::WindowResized> WindowResized = [&](int x, int y, Window& win) {
		Renderer::SetViewport({ 0, 0 }, { x, y });
		proj3D = Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.5, 500);
		VP2D = Math::OrthographicMatrix<float>(0, x, 0, y, -1, 1);
	};
};
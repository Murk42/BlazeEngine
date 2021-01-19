#include "App.h"
#include "MenuScene.h"

void CloseWindowEvent(Window* win)
{
	App::Stop();
}
void ResizeWindowEvent(int w, int h, Window* win)
{
	App& app = App::Instance();

	app.canvasProjection = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);
	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
}

void App::Startup()
{	
	Input::SetEventFunction(InputEvent::WindowClosed, CloseWindowEvent);
	Input::SetEventFunction(InputEvent::WindowSizeChanged, ResizeWindowEvent);

	window.SetSize(Vec2i(800, 400));
	window.SetWindowed(true, false);
	window.ShowWindow(true);

	Renderer::SetClearColor(Color(100, 100, 120));
	Renderer::SetViewport(Vec2i(), window.GetSize());
	Renderer::SetTarget(window);
	Renderer::UseBlending(true);
		
	buttonTexture.SetSettings(TextureSampling::Linear, TextureSampling::Nearest);

	buttonData.texture = &buttonTexture;
	buttonData.textureCenterRect = Recti(26, 37, 44, 14);
	buttonData.texScale = 0.5f;

	//Load tilesMaterial  
	{		
	}

	//Load textMaterial 
	{
		Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/normalText/vertex.glsl");
		Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/normalText/fragment.glsl");
		Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/normalText/geometry.glsl");
		textMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
	}

	//Load ButtonMaterial    
	{
		Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/button/vertex.glsl");
		Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/button/fragment.glsl");
		Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/button/geometry.glsl");
		buttonMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);		
	}

	ChangeScene<MenuScene>();
}
void App::Frame()
{
	Input::Update();

	Renderer::ClearTarget();

	if (activeScene != nullptr)
		activeScene->Frame();

	Renderer::UpdateTarget();

	ProcessLogs();
}
void App::Cleanup()
{
	if (activeScene != nullptr)
	{
		activeScene->Cleanup();
		delete activeScene;
	}
}

void App::ProcessLogs()
{
	Log log;
	while (Logger::GetNextLog(log))
	{
		logList.AddLog(log);
	}

	logList.Refresh();
}
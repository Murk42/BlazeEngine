#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowCloseEvent(Window* win);
static void WindowSizeChangedEvent(int w, int h, Window* win);
static void KeyPressedEvent(Key key, Window* win);

struct ColoredTextMatProp : MaterialProperties<Mat4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
};
struct NormalTextMatProp : MaterialProperties<Mat4f, Texture2D, Vec4f>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
	Property<Vec4f> color = "u_color";
};


class App : public Application<App>
{
public:
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(160, 20));
	Window window;

	std::vector<Color> colors {
		Color(1.0f, 0.0f, 0.0f),
		Color(1.0f, 1.0f, 0.0f),
		Color(0.0f, 1.0f, 0.0f),
		Color(0.0f, 1.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 0.0f),
		Color(1.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f),
	};
	    
	Font font = Font("assets/fonts/VertigoFLF.ttf");
	Material<NormalTextMatProp> normalTextMaterial;
	Material<ColoredTextMatProp> coloredTextMaterial;
	ColoredText text = ColoredText(&font, 50, "Marko :)", colors);
	NormalText dataText;

	Mat4f canvasProjMatrix;	 

	String inputString;
	 
	void Startup() override
	{
		Input::SetEventFunction(InputEvent::WindowSizeChanged, WindowSizeChangedEvent);
		Input::SetEventFunction(InputEvent::WindowClosed, WindowCloseEvent);
		Input::SetEventFunction(InputEvent::KeyPressed, KeyPressedEvent);

		window.SetSize(Vec2i(800, 400));
		window.SetWindowed(true, false); 
		window.ShowWindow(true);

		Renderer::SetClearColor(Color(100, 100, 120));
		Renderer::SetViewport(Vec2i(), window.GetSize());
		Renderer::SetTarget(window);		
		 		
		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/normalText/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/normalText/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/normalText/geometry.glsl");
			normalTextMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);
		}
		{
			Shader vertexShader = Shader(ShaderType::VertexShader, "assets/default/shaders/coloredText/vertex.glsl");
			Shader fragmentShader = Shader(ShaderType::FragmentShader, "assets/default/shaders/coloredText/fragment.glsl");
			Shader geometryShader = Shader(ShaderType::GeometryShader, "assets/default/shaders/coloredText/geometry.glsl");						
			coloredTextMaterial.SetShaders(vertexShader, fragmentShader, geometryShader);			
		}
				
		dataText.SetFont(&font, 40);

		text.transform.parentAlign = Align::TopRight;		
	} 
	
	int FPS = 0;
	int FPScount; 
	Timer FPStimer;

	float dt;
	Timer dtTimer;
	float dtSum;
	float dtAverege;
	void Frame() override
	{		
		dt = dtTimer.Reset();
		dtSum += dt;

		if (FPStimer.GetTime() > 1)
		{
			FPS = FPScount;
			dtAverege = dtSum / FPS;

			FPScount = 0;
			dtSum = 0;

			FPStimer.Reset();
		}
		++FPScount;

		String formatedString = String(format_string, "FPS: %d  dt: %fms dta: %fms rdta: %f", FPS, dt * 1000, dtAverege * 1000, 1.0f / dtAverege);
		dataText.SetString(formatedString);

		Input::Update();					

		  
		Renderer::ClearTarget();

		text.transform.Update();

		dataText.Render(normalTextMaterial, Color(1.0f, 1.0f, 0.0f, 1.0f), canvasProjMatrix);
		text.Render(coloredTextMaterial, canvasProjMatrix * text.transform.mat);		


		Renderer::UpdateTarget();
		 
		ProcessLogs();
	}

	void ProcessLogs()
	{
		bool newLog = false;

		Log log;
		while (Logger::GetNextLog(log))
		{
			logList.AddLog(log);
			newLog = true;
		}

		if (newLog)
			logList.Refresh();
	}
};

static void WindowCloseEvent(Window* win)
{
	BaseApplication::Stop();
}

static void WindowSizeChangedEvent(int w, int h, Window* win)
{
	App& app = App::Instance();	

	Renderer::SetViewport(Vec2i(), Vec2i(w, h));
	app.canvasProjMatrix = Math::OrthographicMatrix<float>(0, w, 0, h, -1, 1);		
}

static void KeyPressedEvent(Key key, Window* win)
{
	App& app = App::Instance();

	switch (key)
	{
	case Key::Backspace: {
		if (app.inputString.Size() > 0)
		{
			app.inputString.Resize(app.inputString.Size() - 1);
			app.text.SetString(app.inputString, app.colors);
		}
		break;
	}
	default: {
		char c = KeyToChar(key);
		if (c != '\0')
		{
			app.inputString += c;
			app.text.SetString(app.inputString, app.colors);			
		}
		break;
	}
	}
}
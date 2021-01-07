#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowCloseEvent(Window* win);
static void WindowSizeChangedEvent(int w, int h, Window* win);
static void KeyPressedEvent(Key key, Window* win);

struct TextMatProp : MaterialProperties<Mat4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
};

class App : public Application<App>
{
public:
	Console::LogList logList = Console::LogList(Vec2i(0, 0), Vec2i(160, 20));
	Window window;
	   
	Font font;  
	Material<TextMatProp> textMaterial;
	TextRenderer text;	
	TextRenderer dataText;

	Mat4f canvasProjMatrix;
	Mat4f textTransform;	

	String inputString;
	 
	void Startup() override
	{
		Input::SetEventFunction(InputEvent::WindowSizeChanged, WindowSizeChangedEvent);
		Input::SetEventFunction(InputEvent::WindowClosed, WindowCloseEvent);
		Input::SetEventFunction(InputEvent::KeyPressed, KeyPressedEvent);

		window.SetSize(Vec2i(800, 600)); 
		window.SetWindowed(true, false);  
		window.ShowWindow(true); 
		
		Renderer::SetTarget(window);		
		 
		font.Load("assets/fonts/VertigoFLF.ttf");
		textMaterial.Load("assets/textMaterial.mat");		
				
		dataText.SetFont(&font, 40);

		text.SetFont(&font, 50); 
		text.SetString("Marko :)");

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

		dataText.SetString(String(format_string, "FPS: %d  dt: %fms dta: %fms rdta: %f", FPS, dt * 1000, dtAverege * 1000, 1.0f / dtAverege));

		Input::Update();					

		  
		Renderer::ClearTarget();

		textMaterial.properties.mvp = canvasProjMatrix;
		textMaterial.properties.texture = dataText.GetTexture();
		Renderer::RenderPointArray(textMaterial, dataText.GetMesh());

		textMaterial.properties.mvp = canvasProjMatrix * textTransform;
		textMaterial.properties.texture = text.GetTexture();
		Renderer::RenderPointArray(textMaterial, text.GetMesh());		


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
	app.textTransform = Math::TranslationMatrix<float>(Renderer::GetViewportSize() - app.text.GetSize());
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
			app.text.SetString(app.inputString);
			app.textTransform = Math::TranslationMatrix<float>(Renderer::GetViewportSize() - app.text.GetSize());
		}
		break;
	}
	default: {
		char c = KeyToChar(key);
		if (c != '\0')
		{
			app.inputString += c;
			app.text.SetString(app.inputString);
			app.textTransform = Math::TranslationMatrix<float>(Renderer::GetViewportSize() - app.text.GetSize());
		}
		break;
	}
	}
}
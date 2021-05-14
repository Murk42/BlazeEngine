#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowCloseEvent(Window* win);
static void WindowSizeChangedEvent(int w, int h, Window* win);
static void KeyPressedEvent(Key key, double lastPress, uint count);
  
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
struct TexturedMatProp : MaterialProperties<Mat4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
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
	    
	Material<NormalTextMatProp> normalTextMaterial; 
	Material<ColoredTextMatProp> coloredTextMaterial;
	Material<TexturedMatProp> texturedMaterial;

	Font font = Font("assets/fonts/VertigoFLF.ttf");
	FontSize smallFontSize = FontSize(&font, 30);
	FontSize mediumFontSize = FontSize(&font, 50);
	FontSize bigFontSize = FontSize(&font, 100);	

	ColoredText text = ColoredText(&mediumFontSize, "Marko :)", colors);
	NormalText dataText = NormalText(&bigFontSize, ""); 

	Mat4f canvasProjMatrix;	  

	String inputString;	

	Texture2D texture;
	Mesh mesh;	

	void Startup() override 
	{
		Input::SetEventFunction<InputEvent::WindowSizeChanged>(WindowSizeChangedEvent);
		Input::SetEventFunction<InputEvent::WindowClosed>(WindowCloseEvent);
		Input::SetEventFunction<InputEvent::KeyPressed>(KeyPressedEvent);

		window.SetSize({ 800, 400 });
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
		{
			Shader vertexShader = { ShaderType::VertexShader, "assets/default/shaders/textured/vertex.glsl" };
			Shader fragmentShader = { ShaderType::FragmentShader, "assets/default/shaders/textured/fragment.glsl" };
			texturedMaterial.SetShaders(vertexShader, fragmentShader);
		}


		{
			Vertex<Vec3f, Vec2f> vertices[4]
			{
				{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
				{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
				{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
				{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } }
			};
			unsigned indices[6]
			{
				0, 1, 2, 1, 2, 3
			};

			mesh.SetVertices(vertices, 4);
			mesh.SetIndicies(indices, 6);
		}

		texture.Load("assets/default/logo.png");
		texturedMaterial.properties.texture = &texture;

		text.transform.parentAlign = Align::TopRight;					
	} 
	
	int FPS = 0;
	int FPScount; 
	Timer FPStimer;
	
	double dtSum;  
	double dtAverege; 
	void Frame() override
	{		
		double dt = Time::GetDeltaTime();
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

		texturedMaterial.properties.mvp = Mat4f::Identity();
		Renderer::RenderTriangles(texturedMaterial, mesh);
		 
		Renderer::UpdateTarget();
		  
		ProcessLogs();

		Time::Update();		
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

static void KeyPressedEvent(Key key, double lastPress, uint count)
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
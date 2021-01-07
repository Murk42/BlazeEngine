#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowClosedFunction(Window* window);
static void WindowResizedFunction(int w, int h, Window* window);
 
struct TexturedUIMatProps : MaterialProperties<Mat4f, TextureArray2D, float>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<TextureArray2D> texture = "u_texture";
	Property<float> radius = "u_radius";
};

struct ColoredUIMatProps : MaterialProperties<Mat4f, float>
{
	Property<Mat4f> mvp = "u_MVP";	
	Property<float> radius = "u_radius";
};

struct TextMatProps : MaterialProperties<Mat4f, Texture2D>
{
	Property<Mat4f> mvp = "u_MVP";
	Property<Texture2D> texture = "u_texture";
};

struct LogCache
{
	int count;
	uint64_t time;
	Log log;
};

enum class ButtonState
{
	Down,
	Up,
	Hovered
};
enum class ButtonChangeState
{
	None,
	Entered,
	Exited,
	Pressed,
	Released
};

class Button
{
	using Vertex = Vertex<Vec2f, Vec2f, Vec4f, float>;
	struct Data
	{
		Vec2f p1;
		Vec2f p2;
		Vec4f color;
		float depth;
	};
	Data* data;
	
	ButtonState state;
	ButtonChangeState changeState;
public:
	Color normalColor;
	Color hoveredColor;
	Color pressedColor;

	Button(Vertex& vertex, Color normal, Color hovered, Color pressed)
		: data((Data*)&vertex), state(ButtonState::Up), changeState(ButtonChangeState::None), normalColor(normal), hoveredColor(hovered), pressedColor(pressed)
	{
	}

	void UpdateState()
	{
		Window* focusedWindow = Input::GetFocusedWindow();
		if (focusedWindow == nullptr)
			return;

		Vec2f mp = Input::GetMousePos();
		mp.y = focusedWindow->GetSize().y - mp.y;

		if (mp.x > data->p1.x && mp.x < data->p2.x && mp.y > data->p1.y && mp.y < data->p2.y)
			if (Input::GetKeyState(Key::MouseLeft) == KeyState::Down)
			{
				if (state != ButtonState::Down)
					changeState = ButtonChangeState::Pressed;
				else
					changeState = ButtonChangeState::None;
				state = ButtonState::Down;
				data->color = pressedColor.ToVector();
			}
			else
			{				
				if (state == ButtonState::Up)
					changeState = ButtonChangeState::Entered;
				else if (state == ButtonState::Down)
					changeState = ButtonChangeState::Released;
				else
					changeState = ButtonChangeState::None;
				state = ButtonState::Hovered;
				data->color = hoveredColor.ToVector();
			}
		else
		{
			if (state != ButtonState::Up)
				changeState = ButtonChangeState::Exited;
			else
				changeState = ButtonChangeState::None;
			state = ButtonState::Up;
			data->color = normalColor.ToVector();
		}		
	}

	ButtonState GetState() const { return state; }
	ButtonChangeState GetChangeState() const { return changeState; }
};

class Target
{
	enum class TargetType
	{
		Window,
		Texture,
	};

	void* targetPtr;
	TargetType type;

public:
	Mat4f projectionMatrix;

	Target(Window& window)
		: targetPtr(&window), type(TargetType::Window)
	{

	}
	Target(Texture2D& texture)
		: targetPtr(&texture), type(TargetType::Texture)
	{

	}

	virtual void Update() { }

	void UpdateTarget()
	{
		Vec2i size;
		if (type == TargetType::Window)
		{
			Window& window = *(Window*)targetPtr;
			Renderer::SetTarget(window);
			size = window.GetSize();

		}
		Renderer::SetViewport(Vec2i(0, 0), size);
		Renderer::ClearTarget();

		Update();

		Renderer::UpdateTarget();
	}
};

class View
{
public:
	View()
	{

	}


};

class App : public Application<App>
{
public:		
	std::vector<LogCache> logCache;
	Console::List logList = Console::List(Vec2i(0, 0), Vec2i(160, 20));
	Console::Frame stateFrame = Console::Frame(Vec2i(0, 22), Vec2i(50, 20));	

	Window mainWindow;
	Window utilityWindow;

	Font font;
	Material<ColoredUIMatProps> coloredUIMaterial;	
	Material<TextMatProps> textMaterial;
	TextureArray2D texture; 

	Vertex<Vec2f, Vec2f, Vec4f, float> uiVertices[3]{
				{ { 0, 0 }, { 800, 400 }, { 0.2f, 0.2f, 0.2f, 1.0f }, 0.2f },
				{ { 5, 5 }, { 105, 45 }, { 0.3f, 0.3f, 0.3f, 1.0f }, 0.1f },
				{ { 110, 5 }, { 210, 45 }, { 0.3f, 0.3f, 0.3f, 1.0f }, 0.1f },
	};

	Button button1 = Button(uiVertices[1], Color(0.3f), Color(0.28f), Color(0.25f));
	Button button2 = Button(uiVertices[2], Color(0.3f), Color(0.28f), Color(0.25f));

	Mesh uiMesh;

	TextRenderer text;			

	void Startup() override
	{					
		Input::SetEventFunction(InputEvent::WindowClosed, WindowClosedFunction);
		Input::SetEventFunction(InputEvent::WindowResized, WindowResizedFunction);					

		mainWindow.SetSize(Vec2i(960, 540));
		mainWindow.SetWindowed(true, false);
		mainWindow.ShowWindow(true);

		utilityWindow.SetSize(Vec2i(800, 400));
		utilityWindow.SetWindowed(true, false);
		utilityWindow.ShowWindow(true);
		 
		{
			Shader vertexShader = Shader(VertexShader, "assets/UI/Shaders/ColoredUI/vertex.glsl");
			Shader geometryShader = Shader(GeometryShader, "assets/UI/Shaders/ColoredUI/geometry.glsl");
			Shader fragmentShader = Shader(FragmentShader, "assets/UI/Shaders/ColoredUI/fragment.glsl");
			coloredUIMaterial.SetShaders(vertexShader, geometryShader, fragmentShader);
		}

		coloredUIMaterial.properties.radius = 5;

		{
			Shader vertexShader = Shader(VertexShader, "assets/UI/Shaders/Text/vertex.glsl");
			Shader geometryShader = Shader(GeometryShader, "assets/UI/Shaders/Text/geometry.glsl");
			Shader fragmentShader = Shader(FragmentShader, "assets/UI/Shaders/Text/fragment.glsl");
			textMaterial.SetShaders(vertexShader, geometryShader, fragmentShader);
		}

		texture.Load("assets/Sprite.png", Vec2i(128, 128));
		font.Load("assets/UI/Fonts/VertigoFLF.ttf"); 				

		uiMesh.SetVertices(uiVertices, 3);
		text.SetMaterial(&textMaterial, textMaterial.properties.texture);
	}

	void Cleanup() override
	{				
	} 	
	
	uint FPScount = 0; 
	uint FPS = 0;
	Timer fpsTimer;

	Timer dtTimer;
	float dt = 0;	
	void Frame() override
	{
		dt = dtTimer.Reset();

		++FPScount;
		if (fpsTimer.GetTime() > 1)
		{
			FPS = FPScount;
			FPScount = 0;
			fpsTimer.Reset();

			text.SetText(&font, 50, String(format_string, "FPS: %d", FPS));

			stateFrame.Clear();
			stateFrame.Write(String(format_string, "FPS: %d", FPS));
			stateFrame.Refresh();
		}

		//Processing input;
		{
			Input::Update();
			Window* focusedWindow = Input::GetFocusedWindow();

			if (focusedWindow != nullptr)
			{
				if (Input::GetKeyState(Key::F11) == KeyState::Pressed)
					if (focusedWindow->IsFullscreen())
						focusedWindow->SetWindowed(true, false);
					else
						focusedWindow->SetFullscreen();
			}
		}		



		Renderer::SetTarget(mainWindow);
		Renderer::SetViewport(Vec2i(), mainWindow.GetSize());
		Renderer::ClearTarget();

		//Render to main window
		{
			button1.UpdateState();
			button2.UpdateState();
			uiMesh.ChangeVertices(uiVertices, 3, 0);

			if (button1.GetState() == ButtonState::Down)
			{
				uiVertices[1].GetValue<0>() += Input::GetMouseMovement() * Vec2i(1, -1);
				uiVertices[1].GetValue<1>() += Input::GetMouseMovement() * Vec2i(1, -1);
			}
			 
			Vec2f size = Renderer::GetViewportSize();
			coloredUIMaterial.properties.mvp = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1, 1);

			Renderer::RenderPointArray(coloredUIMaterial, uiMesh);
		}

		Renderer::UpdateTarget();

		Renderer::SetTarget(utilityWindow);
		Renderer::SetClearColor(Color(20, 20, 20));
		Renderer::SetViewport(Vec2i(), utilityWindow.GetSize());
		Renderer::ClearTarget();

		//Render to utilityWindow
		{
			Vec2f size = Renderer::GetViewportSize();
			textMaterial.properties.mvp = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1, 1);
			text.Render(Vec2i(0, 0));
		}

		Renderer::UpdateTarget();

		ProcessLogs();			
	}

	void ProcessLogs()
	{		
		LogCache cache;
		while (Logger::GetNextLog(cache.log))
		{
			cache.time = Time::GetWorldTimeSeconds();

			bool found = false;
			int i = 0;
			for (; i < logCache.size(); ++i)
				if (logCache[i].log == cache.log)
				{
					++logCache[i].count;
					logCache[i].time = cache.time;
					found = true;
					break;
				}

			if (found)
			{
				auto& list = logList.GetList();
				list[i];

				int h, m, s;
				s = logCache[i].time % 60;
				m = (logCache[i].time / 60) % 60;
				h = (logCache[i].time / 3600) % 24;

				const char* color = nullptr;
				switch (logCache[i].log.type)
				{
				case LogType::Error: color = F_RED; break;
				case LogType::Warning: color = F_YELLOW; break;
				default: color = F_WHITE; break;
				}

				list[i] = color + String(format_string, "(%d) [%d:%d:%d] ", logCache[i].count, h, m, s) + logCache[i].log.source + ": " + logCache[i].log.message;
			}
			else
			{								
				cache.count = 1;
				logCache.emplace_back(cache);
				int h, m, s;
				s = cache.time % 60;
				m = (cache.time / 60) % 60;
				h = (cache.time / 3600) % 24;

				const char* color = nullptr;
				switch (cache.log.type)
				{
				case LogType::Error: color = F_RED; break;
				case LogType::Warning: color = F_YELLOW; break;
				default: color = F_WHITE; break;
				}

				logList.Add(color + String(format_string, "[%d:%d:%d] ", h, m, s) + cache.log.source + ": " + cache.log.message);
			}			
		}

		if (cache.log.message.Size() != 0)
		{
			logList.Clear();
			logList.Refresh();
		}
	}
};  

static void WindowClosedFunction(Window* window)
{
	App& app = App::Instance();
	if (window == &app.mainWindow)
	{
		window->ShowWindow(false);
		BaseApplication::Stop();
	}
}

static void WindowResizedFunction(int w, int h, Window* window)
{
	App& app = App::Instance();
	Logger::AddLog(LogType::Message, __FUNCTION__, "Prozor je preuvelican");
}
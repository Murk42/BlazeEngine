#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Scene.h"

static void SetupEvents();
class App : public Application<App>
{
public:
	Console::List logList = Console::List({ 0, 0 }, { 100, 20 });
	Console::Frame frame = Console::Frame({ 0, 22 }, { 50, 20 });
	Window win;	

	void Startup() override
	{
		win.ShowWindow(true);
		Renderer::SetTarget(win);		

		SetupEvents();

		SceneManager::LoadScene<MainScene>();
		SceneManager::RunScene<MainScene>();		
	}

	void Cleanup() override
	{

	}

	Timer t;
	double dt;
	uint fps = 0;
	uint fpsCount = 0;
	Timer fpsTimer; 	

	void Frame() override
	{
		fpsCount++;
		if (fpsTimer.GetTime() >= 1.0)
		{
			fps = fpsCount;
			fpsCount = 0;
			fpsTimer.Reset(); 
		}

		dt = t.Reset();

		Input::Update();	

		SceneManager::Update(dt);

		frame.Clear();
		frame.Write("frame time: " + String(format_string, "%f", dt), Vec2i(0, 0));
		frame.Write("FPS: " + String(format_string, "%i", fps), Vec2i(0, 1));
		frame.Refresh();

		PrintLogs();
	}

	void PrintLogs()
	{
		Log log;
		while (Logger::GetNextLog(log))
			logList.Add(log.source + ": " + log.message);

		if (log.message.Size() != 0)
		{
			logList.Clear();
			logList.Refresh();
		}
	}	
};

static void WindowResizeFunc(int x, int y, Window* win)
{
	Renderer::SetViewport(Vec2i(), Vec2i(x, y));
	
}

static void WindowCloseFunc(Window* win)
{
	BaseApplication::Stop();
}

static void SetupEvents()
{
	WindowResizeFunc(App::Instance()->win.GetSize().x, App::Instance()->win.GetSize().y, &App::Instance()->win);
	Input::AddEventFunction(WindowResized, WindowResizeFunc);
	Input::AddEventFunction(WindowClose, WindowCloseFunc);
}
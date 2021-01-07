#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void SetupEvents();
class App : public Application<App>
{
public:
	Console::List logList = Console::List({ 0, 0 }, { 100, 20 });
	Window win;

	void Startup() override
	{
		win.ShowWindow(true);
		Renderer::SetTarget(win);

		SetupEvents();

		Bitmap spectrumBM;
		spectrumBM.Load("assets/Spectrum.png"); 
		  
		Bitmap outBM; 
		outBM.Create(Vec2i(spectrumBM.GetSize().x, 256)); 

		Colorb* spectrum = spectrumBM.GetPixels();
		Colorb* out = outBM.GetPixels();
		memset(out, 0, outBM.GetSize().x * 255 * sizeof(Colorb)); 

		for (int x = 0; x < spectrumBM.GetSize().x; ++x)
		{ 
			out[x + spectrum[x].r * 256].r = 255;
			out[x + spectrum[x].g * 256].g = 255;
			out[x + spectrum[x].b * 256].b = 255;
			for (int y = 0; y < 256; ++y)
				out[x + y * 256].a = 255;
		}

		outBM.Save("assets/Out.png"); 
	}

	void Cleanup() override
	{

	}

	void Frame() override
	{
		Input::Update();		

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

#include "Events.h"

static void SetupEvents()
{
	WindowResizeFunc(App::Instance()->win.GetSize().x, App::Instance()->win.GetSize().y, &App::Instance()->win);
	Input::AddEventFunction(WindowResized, WindowResizeFunc);
	Input::AddEventFunction(WindowClose, WindowCloseFunc);
}
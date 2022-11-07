#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class MainEventHandler :
	public EventHandler<Event::MouseMotion>
{
public:
	void OnEvent(Event::MouseMotion event)
	{

	}
};

class ConsoleStream
{
	struct CharData
	{
		UnicodeChar value;
		ColorRGBf color;
	};
	std::vector<std::vector<CharData>> lines;
	Vec2i cursor;

	void AddChar(UnicodeChar ch, ColorRGBf color)
	{
		if (lines[cursor.y].size() <= cursor.x)
			lines[cursor.y].resize(cursor.x + 1);

		lines[cursor.y][cursor.x] = { ch, color };
	}
public:
	void Write(StringViewUTF8 text)
	{
		ColorRGBf color;

		size_t state = 0;
		size_t charsEntered = 0;		

		auto* line = &lines[cursor.y];
		auto it = text.begin();
		while (it != text.end())		
		{		
			UnicodeChar ch = *it;

			switch (state)
			{
			case 0:
				if (ch.Value() == '%')
				{
					state = 1;
				}
				if (ch.Value() == '\n')
				{
					++cursor.y;
					if (lines.size() <= cursor.y)
						lines.resize(cursor.y + 1);
				}
				else
				{					
					AddChar(ch, color);
					++cursor.x;
				}
				break;
			case 1:
				if (ch.Value() == 'c')
				{
					state = 2;
				}
				else if (ch.Value() == '%')
				{
					AddChar('%', color);
				}
				break;
			case 2:

				break;
			}

			++it;
		}
	}
	void WriteLine(StringViewUTF8 text);
};

CLIENT_API void Setup()
{
	LogListener listener;
	listener.StartListening();
	listener.SupressLogs(true);

	Window window{ "Blaze Console", Vec2i(640, 360) };		
	window.SetWindowed(false, false);
	window.ShowWindow(true);	

	window.SetOpacity(1.0f);

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());
	Renderer::SetClearColor(0x333333ff);

	ResourceSystem::ResourceManager resourceManager;
	resourceManager.AddResource("Default font", &Graphics::GetDefaultFont());

	UI::UIScene scene;		
	scene.SetResourceManager(&resourceManager);
	scene.Load("assets/scene");				
	

	String str;
	while (!Input::GetKeyState(Key::Escape).down)
	{
		Input::Update();		

		scene.Update(); 

		auto logs = listener.GetLogs();
		for (auto log : logs)
			str += log.GetFunctionName() + "\n";
		listener.ClearLogs();

		Renderer::ClearTarget( );		

		scene.Render();		
		Graphics::Write((StringUTF8)str, Vec2f(0, window.GetSize().y - 15), 14, 0xffffffff);

		Renderer::SwapWindowBuffers();		
	}
	listener.StopListening();
}

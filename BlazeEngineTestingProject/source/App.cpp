/*#include "Globals.h"

#include "UI Scenes/CreateElementMenu.h"
#include "UI Scenes/CreateSceneMenu.h"
#include "UI Scenes/ElementPropertiesMenu.h"
#include "UI Scenes/GameImage.h"
#include "UI Scenes/SceneListMenu.h"
#include "UI Scenes/CreateLayerMenu.h"

int CalculateFPS()
{
	static int fps_count;
	static int fps;
	static TimePoint last_time_point = TimePoint::GetWorldTime();

	TimePoint now = TimePoint::GetWorldTime();
	if (now - last_time_point > 1.0f)
	{
		fps = fps_count;
		last_time_point = now;
		fps_count = 0;
	}
	fps_count++;

	return fps;
}

class MainEventHangler :
	public EventHandler<Event::ViewportChanged>,
	public EventHandler<Event::WindowResized>
{
public:
	void OnEvent(Event::WindowResized event) override
	{
		Renderer::SetViewport({}, event.size);

		Vec2i gameImageSize(event.size.x - 640, (event.size.x - 640) / 16 * 9);

		mainScene->GetElementBase("gameImage")->SetSize((Vec2f)gameImageSize);
	}
	void OnEvent(Event::ViewportChanged event) override
	{		
		Graphics::Set3DProjectionMatrix(90, 0.1, 1000);

		uiManager.SetViewport(Vec2i(), event.size);		
	}
};

void SetupResources()
{
	ResourceSystem::ResourceTypeRegistry registry;
	registry.RegisterCoreTypes();
	registry.RegisterType<UI::UIScene>();

	resourceManager.SetResourceTypeRegistry(registry);
	gameResourceManager.SetResourceTypeRegistry(registry);
	resourceStorage.SetResourceManager(&resourceManager);
	gameResourceStorage.SetResourceManager(&gameResourceManager);

	resourceManager.AddResource("Default font", &Graphics::GetDefaultFont());	

	{
		fontResolution18 = Graphics::GetDefaultFont().CreateFontResolution(18, FontResolutionRenderType::Antialiased);
		fontResolution18->LoadCharacters(0, 128);
		fontResolution18->CreateAtlas();
	}
	{
		fontResolution14 = Graphics::GetDefaultFont().CreateFontResolution(14, FontResolutionRenderType::Antialiased);
		fontResolution14->LoadCharacters(0, 128);
		fontResolution14->CreateAtlas();
	}
	{
		fontResolution8 = Graphics::GetDefaultFont().CreateFontResolution(10, FontResolutionRenderType::Antialiased);
		fontResolution8->LoadCharacters(0, 128);
		fontResolution8->CreateAtlas();
	}
	{
		auto* tex = resourceStorage.CreateResource<Graphics::Core::Texture2D>("plusIcon").value;
		tex->Load("assets/plusIcon.png");
		tex->SetSettings({});
	}
}
void SetupUI()
{	
	uiManager.CreateLayer("main");

	mainScene = resourceStorage.CreateResource<UI::UIScene>();
	SetupUIScene(mainScene, &uiManager);

	gameUIManager.CreateLayer("base");

	SceneListMenu::Setup();
	GameImage::Setup();
	ElementPropertiesMenu::Setup();
	CreateElementMenu::Setup();
	CreateSceneMenu::Setup();
	CreateLayerMenu::Setup();
}

CLIENT_API void Setup()
{
	MainEventHangler eventHandler;

	EventManager::AddEventHandler<Event::WindowResized>(eventHandler);
	EventManager::AddEventHandler<Event::ViewportChanged>(eventHandler);

	SetupUIManager(&uiManager);
	SetupUIManager(&gameUIManager);
	SetupResources();
	SetupUI();
	
	Window window{ "Blaze engine application", Vec2i(1280, 720) };
	window.SetWindowed(true, false);	
	window.ShowWindow(true);
	window.SetMinimumSize({ 1000, 500 });	

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());

	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		int FPS = CalculateFPS();

		Input::Update();

		{
			auto* elementLayer = mainScene->GetElement<UI::DropdownSelection>("createElementMenu_elementLayer").value;
			std::vector<StringUTF8> o;
		
			auto& layers = gameUIManager.GetLayers();
		
			for (auto l : layers)
				o.push_back((StringUTF8)l.name);
		
			elementLayer->SetOptions(o);
		}

		gameUIManager.Update();
		uiManager.Update();

		if (Input::GetKeyState(MouseKey::MouseLeft).pressed)
		{			
			UI::UIElement* el = gameUIManager.GetBlockingElement();
			ElementPropertiesMenu::SelectElement(el);
		}

		{
			Renderer::ClearTarget();
			
			gameUIManager.Render();

			if (Input::GetKeyState(Key::Q).down && Input::GetKeyState(Key::LShift	).down)
				gameUIManager.DrawDebugLines();

			uiManager.Render();

			if (Input::GetKeyState(Key::Q).down)
				uiManager.DrawDebugLines();
			
			Renderer::SwapWindowBuffers();
		}
	}

	{
		const auto& scenes = gameResourceStorage.GetResourceList<UI::UIScene>();
		while (!scenes.empty())
			gameResourceStorage.DestroyResource<UI::UIScene>((UI::UIScene*)*scenes.begin());
	}

	{
		const auto& scenes = resourceStorage.GetResourceList<UI::UIScene>();
		while (!scenes.empty())
			resourceStorage.DestroyResource<UI::UIScene>((UI::UIScene*)*scenes.begin());
	}
}*/

void* operator new(size_t size) {
	return Memory::Allocate(size);	
}
void operator delete(void* ptr) {
	Memory::Free(ptr);
}

CLIENT_API void Setup()
{
	int* a = new int();
	delete a;

	
	//bool exit = false;
	//Logger::SetOutputFile("log.txt");
	//
	//Window window;
	//window.ShowWindow(true);
	//
	//Renderer::SetTarget(window);	
	//
	//UI::UIManager manager;
	//manager.SetElementTypeRegistry(UI::UIElementTypeRegistry::CoreRegistry());	
	//
	//manager.CreateLayer("main");

	//UI::TextField el2;
	//el2.EventHandler<Event::TextInput>::~EventHandler();

	//el2.SetSize(Vec2f(400, 30));
	//el2.SetValidateTextFunction([&](StringUTF8 text) {				
	//	bool dot = false;
	//
	//	for (auto it = text.begin(); it != text.end(); ++it)
	//	{
	//		char ch = it.ToUnicode().Value();
	//		if (ch == '.')
	//			if (dot)
	//				return false;
	//			else
	//				dot = true;
	//		else if (!isdigit(ch))
	//			return false;
	//			
	//	}
	//
	//	return true;
	//	});
	//el2.AddValueEnteredFunction([&]() {
	//	float value;
	//	if (StringUTF8::ConvertTo<float>(el2.GetText(), value))
	//		el2.SetText(StringUTF8::Convert(value));
	//	else
	//		el2.SetText("");
	//	});

	//while (!exit)
	//{
	//	Input::Update();
	//
	//	if (Input::GetKeyState(Key::Escape).pressed)
	//		exit = true;
	//
	//	Renderer::ClearTarget();
	//
	//	manager.Update();
	//	manager.Render();
	//
	//	Renderer::SwapWindowBuffers();
	//}
}
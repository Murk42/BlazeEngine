/*
#include "Globals.h"

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
		Graphics::Core::SetViewport({}, event.size);

		Vec2i gameImageSize(event.size.x - 640, (event.size.x - 640) / 16 * 9);

		globals->mainScene->GetElementBase("gameImage")->SetSize((Vec2f)gameImageSize);
	}
	void OnEvent(Event::ViewportChanged event) override
	{		
		Graphics::Set3DProjectionMatrix(90, 0.1, 1000);

		globals->uiManager.SetViewport(Vec2i(), event.size);
	}
};

void SetupResources()
{
	ResourceSystem::ResourceTypeRegistry registry;
	registry.RegisterCoreTypes();
	registry.RegisterType<UI::UIScene>();

	globals->resourceManager.SetResourceTypeRegistry(registry);
	globals->gameResourceManager.SetResourceTypeRegistry(registry);
	globals->resourceStorage.SetResourceManager(&globals->resourceManager);
	globals->gameResourceStorage.SetResourceManager(&globals->gameResourceManager);

	globals->resourceManager.AddResource("Default font", &Graphics::GetDefaultFont());

	{
		globals->fontResolution18 = Graphics::GetDefaultFont().CreateFontResolution(18, FontResolutionRenderType::Antialiased);
		globals->fontResolution18->LoadCharacters(0, 128);
		globals->fontResolution18->CreateAtlas();
	}
	{
		globals->fontResolution14 = Graphics::GetDefaultFont().CreateFontResolution(14, FontResolutionRenderType::Antialiased);
		globals->fontResolution14->LoadCharacters(0, 128);
		globals->fontResolution14->CreateAtlas();
	}
	{
		globals->fontResolution8 = Graphics::GetDefaultFont().CreateFontResolution(10, FontResolutionRenderType::Antialiased);
		globals->fontResolution8->LoadCharacters(0, 128);
		globals->fontResolution8->CreateAtlas();
	}
	{
		auto* tex = globals->resourceStorage.CreateResource<Graphics::Core::Texture2D>("plusIcon").value;
		tex->Load("assets/plusIcon.png");
		tex->SetSettings({});
	}
}
void SetupUI()
{	
	globals->uiManager.CreateLayer("main");

	globals->mainScene = globals->resourceStorage.CreateResource<UI::UIScene>();
	SetupUIScene(globals->mainScene, &globals->uiManager);

	globals->gameUIManager.CreateLayer("base");

	SceneListMenu::Setup();
	GameImage::Setup();
	ElementPropertiesMenu::Setup();
	CreateElementMenu::Setup();
	CreateSceneMenu::Setup();
	CreateLayerMenu::Setup();
}

CLIENT_API void Setup()
{
	globals = new Globals;

	MainEventHangler eventHandler;

	EventManager::AddEventHandler<Event::WindowResized>(eventHandler);
	EventManager::AddEventHandler<Event::ViewportChanged>(eventHandler);

	SetupUIManager(&globals->uiManager);
	SetupUIManager(&globals->gameUIManager);
	SetupResources();
	SetupUI();
	
	Window window{ "Blaze engine application", Vec2i(1280, 720) };
	window.SetWindowed(true, false);	
	window.ShowWindow(true);
	window.SetMinimumSize({ 1000, 500 });	

	Graphics::Core::SetTarget(window);
	Graphics::Core::SetViewport({}, window.GetSize());

	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		int FPS = CalculateFPS();

		Input::Update();


		{
			auto* elementLayer = globals->mainScene->GetElement<UI::DropdownSelection>("createElementMenu_elementLayer").value;
			std::vector<StringUTF8> o;
		
			auto& layers = globals->gameUIManager.GetLayers();
		
			for (auto l : layers)
				o.push_back((StringUTF8)l.name);
		
			elementLayer->SetOptions(o);
		}

		globals->uiManager.Update();
		globals->gameUIManager.Update();
		GameImage::Update();
		ElementPropertiesMenu::Update();

		{
			Graphics::Core::ClearTarget();
			
			globals->gameUIManager.Render();

			if (Input::GetKeyState(Key::Q).down && Input::GetKeyState(Key::LShift	).down)
				globals->gameUIManager.DrawDebugLines();

			globals->uiManager.Render();

			if (Input::GetKeyState(Key::Q).down)
				globals->uiManager.DrawDebugLines();
			
			Graphics::Core::SwapWindowBuffers();
		}
	}

	{
		const auto& scenes = globals->gameResourceStorage.GetResourceList<UI::UIScene>();
		while (!scenes.empty())
			globals->gameResourceStorage.DestroyResource<UI::UIScene>((UI::UIScene*)*scenes.begin());
	}

	{
		const auto& scenes = globals->resourceStorage.GetResourceList<UI::UIScene>();
		while (!scenes.empty())
			globals->resourceStorage.DestroyResource<UI::UIScene>((UI::UIScene*)*scenes.begin());
	}

	delete globals;
}*/

struct Component
{
	size_t typeID;
};


class ElementHandler
{
public:
};

class Element
{
public:
	ElementHandler* handler;
};

class ComponentTypeRegistry
{
public:
	template<typename T>
	size_t GetComponentTypeID()
	{
		return 0;
	}
};

class Scene
{
	class ComponentContainer
	{
		size_t elementSize;
		size_t size;
		void* data;

		ComponentContainer()
			: elementSize(-1), size(0), data(nullptr)
		{

		}
		
		void* New()
		{
			size += elementSize;
			void* old = data;
			data = Memory::Allocate(size);
			memcpy(data, old, size - elementSize);
			Memory::Free(old);
		}
		void Delete(void* ptr)
		{
			size -= elementSize;
			void* old = data;
			data = Memory::Allocate(size);
			size_t offset = (byte*)ptr - (byte*)old;
			memcpy(data, old, offset);
			memcpy((byte*)data + offset, (byte*)old + offset + elementSize, size - offset);
			Memory::Free(old);
		}
	};

	ComponentTypeRegistry registry;
	std::vector<Element> elements;
	std::vector<ComponentContainer> components;	

	template<typename C>
	void AddComponent(Element* element)
	{
		components[registry.GetComponentTypeID<C>()];
		element->components[element->componentCount++];
			
	}
public:
	template<typename H>
	Element* CreateElement()
	{
		Element* element = &elements.emplace_back();
		(AddComponent<T>(element), ...);
	}
};

class TextComponent
{

};
class ButtonComponent
{

};
class PanelComponent
{

};

class TextButtonHandler
{
public:
	TemplateGroup<TextComponent, ButtonComponent, PanelComponent> Copmonents;
};

CLIENT_API void Setup()
{	
	Scene scene;
	auto* element = scene.CreateElement<TextButtonHandler>();
	element->
}

//build "" "C:\Users\Marko\Desktop\app" "app.exe" -guiOpenFile -p:Configuration=Debug -p:Platform=x64
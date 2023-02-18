using namespace Blaze::ECS;

#include "UI/Text.h"
#include "UI/SceneFileParser.h"
#include "UI/NameReferenceComponent.h"

class Events : public EventHandler<Event::WindowResized>
{
public:
	void OnEvent(Event::WindowResized event)
	{
		Graphics::Core::SetViewport({ }, event.size);
		//s->SetProjectionMatrix(Math::OrthographicMatrix<float>(0, event.size.x, 0, event.size.y, -100, 100));
	}
};

void PrintEntityInfo(Entity* entity)
{
	auto& registry = entity->GetScene()->GetManager()->GetRegistry();
	auto& components = entity->GetComponents();

	Console::Write("Entity: \n");

	for (auto component : components)
	{
		auto typeData = registry.GetComponentTypeData(component->GetTypeIndex());
		Console::Write("\t" + typeData.typeName + "\n");
	}
}

void AddElement(SceneFileParser::Element* element, Scene* scene, UI2::Transform* parent)
{
	auto& registry = scene->GetManager()->GetRegistry();
	uint typeIndex = registry.GetComponentTypeIndex(element->typeName);
	
	Component* components[3];

	auto entity = scene->Create({ 
		registry.GetComponentTypeIndex<NameReferenceComponent>(), 
		registry.GetComponentTypeIndex<UI2::Transform>(), 
		typeIndex
	}, components);

	auto* transform = entity->GetComponent<UI2::Transform>();
	transform->anchor = parent;

	for (auto& p : element->properties)
	{
		bool parsed = false;
		for (auto& component : components)
		{
			System* system = scene->GetManager()->GetSystem(component->GetTypeIndex());
			if (system->SetProperty(component, p.name, p.value))
			{
				parsed = true;
				break;
			}
		}

		if (!parsed)
			Logger::AddLog(BLAZE_INFO_LOG("Client", "Property not parsed: " + p.name + " = " + p.value));		
	}

	for (auto& child : element->children)
		AddElement(&child, scene, transform);
}

void LoadSceneFile(Path path, Scene* scene)
{
	SceneFileParser sfp;
	sfp.Load(path);
	for (auto& e : sfp.elements)
		AddElement(&e, scene, nullptr);
}



CLIENT_API void Setup()
{		
	Logger::SetOutputFile("log.txt");

	Events e;
	EventManager::AddEventHandler<Event::WindowResized>(e);

	ResourceSystem::ResourceTypeRegistry resourceRegistry;
	resourceRegistry.RegisterType<FontResolution>();
	ResourceSystem::ResourceManager resourceManager;	
	resourceManager.SetResourceTypeRegistry(resourceRegistry);

	ComponentTypeRegistry registry;
	registry.RegisterType<UI2::Transform>();
	registry.RegisterType<UI2::Text>();
	registry.RegisterType<NameReferenceComponent>();

	Manager manager;
	manager.SetRegistry(registry);
	manager.resourceManager = &resourceManager;

	Scene scene;
	scene.SetManager(&manager);

		
	FontResolution* res = Graphics::GetDefaultFont().CreateFontResolution(32, FontResolutionRenderType::Antialiased);
	res->LoadCharacters(0, 128);
	res->CreateAtlas();
	res->GetAtlas().GenerateMipmaps();
	Graphics::GetTextRenderer().SetFontResolution(res);		
	resourceManager.AddResource<FontResolution>("DefaultFont", res);

	LoadSceneFile("assets/scene.bui", &scene);

	Window window;
	window.SetWindowed(true, false);
	window.ShowWindow(true);
	
	Graphics::Core::SetViewport({ }, window.GetSize());
	Graphics::Core::SetTarget(window);

	//auto* text = manager.GetSystem<NameReferenceComponent>()->GetComponent("lolic")->GetEntity()->GetComponent<UI2::Text>();
	//text->SetText("je najbolji :)");

	while (!Input::GetKeyState(Key::Escape).down)
	{
		Input::Update();

		Graphics::Core::ClearTarget();
		
		scene.UpdateSystem<UI2::Transform>();
		scene.UpdateSystem<UI2::Text>();

		Graphics::Core::SwapWindowBuffers();
	}
}
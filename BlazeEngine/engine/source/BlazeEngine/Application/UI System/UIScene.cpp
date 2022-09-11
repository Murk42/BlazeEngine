#include "BlazeEngine/Application/UI System/UIScene.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Core/MemoryManager.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"


#include "UISceneParser.h"

namespace Blaze::UI
{
	UIScene::UIScene()
		: typeCount(0), managers(nullptr), parsingData(nullptr)
	{
		SetElementTypeRegistry(UIElementTypeRegistry::CoreRegistry());
		SetupElementManagers();
	}
	UIScene::~UIScene()
	{
		for (auto& layer : layers)
		{
			for (int i = 0; i < typeCount; ++i)
			{
				const auto& typeData = manager.GetElementTypeRegistry().GetElementTypeData(i);
				for (auto& element : layer.arrays[i].elements)
				{
					typeData.destruct(element);
					Memory::Free(element);
				}
			}

			delete[] layer.arrays;
		}

		CleanupElementManagers();

		for (int i = 0; i < typeCount; ++i)
		{
			const auto& typeData = manager.GetElementTypeRegistry().GetElementTypeData(i);
			typeData.destructManager(managers[i]);
			Memory::Free(managers[i]);
		}
		delete[] managers;
	}

	void UIScene::SetElementTypeRegistry(const UIElementTypeRegistry& registry)
	{		
		CleanupElementManagers();

		for (int i = 0; i < typeCount; ++i)
		{
			const auto& typeData = manager.GetElementTypeRegistry().GetElementTypeData(i);
			typeData.destructManager(managers[i]);
			Memory::Free(managers[i]);
		}
		delete[] managers;
		delete[] parsingData;

		parsingData = nullptr;

		typeCount = registry.GetElementTypeCount();
		manager.SetElementTypeRegistry(registry);

		managers = new UIBaseElementManager * [typeCount];

		for (int i = 0; i < typeCount; ++i)
		{
			const auto& typeData = registry.GetElementTypeData(i);
			managers[i] = (UIBaseElementManager*)Memory::Allocate(typeData.managerSize);
			typeData.constructManager(managers[i]);

			manager.SetElementManager(managers[i], i);
		}

		SetupElementManagers();
	}

	void UIScene::SetResourceManager(Resource::ResourceManager* resourceManager)
	{
		this->resourceStorage.SetResourceManager(resourceManager);
	}

	uint UIScene::CreateLayer()
	{
		uint layer = manager.CreateLayer();
		LayerStorage& storage = layers.emplace_back();
		storage.arrays = new TypeStorage[manager.GetElementTypeCount()];

		return layer;
	}
	void UIScene::AddElementParsingData(const UIElementParsingData& parsingData, uint type)
	{
		if (this->parsingData == nullptr)
			this->parsingData = new UIElementParsingData[typeCount];

		this->parsingData[type] = parsingData;
	}
	void UIScene::AddElementParsingData(UIElementParsingData&& parsingData, uint type)
	{
		if (this->parsingData == nullptr)
			this->parsingData = new UIElementParsingData[typeCount];

		this->parsingData[type] = std::move(parsingData);
	}
	void UIScene::AddCoreElementsParsingData()
	{
		AddElementParsingData<UI::Button>();
		AddElementParsingData<UI::Image>();
		AddElementParsingData<UI::Panel>();
		AddElementParsingData<UI::Text>();
		AddElementParsingData<UI::TexturedPanel>();
	}
	void UIScene::AddEventFunction(StringView name, EventFunction func)
	{
		functions.insert({ name, func });
	}
	ResultValue<UIElement*> UIScene::CreateElement(uint typeIndex, uint layer)
	{
		const auto& typeData = manager.GetElementTypeRegistry().GetElementTypeData(typeIndex);

		auto& elements = layers[layer].arrays[typeIndex].elements;
		void*& element = elements.emplace_back();
		element = Memory::Allocate(typeData.size);
		typeData.construct(element);

		manager.AddElement((UIElement*)element, typeIndex, layer);
		return (UIElement*)element;
	}
	ResultValue<UIElement*> UIScene::CreateElement(StringView name, uint typeIndex, uint layer)
	{		
		auto p = elementNames.insert({ name, { nullptr, 0 } });
		if (p.second)
		{
			UIElement* element = CreateElement(typeIndex, layer);

			p.first->second = { element, typeIndex };

			return element;
		}
		else
			return { nullptr, Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, 
				"Blaze Engine", "Trying to create a element, but there already exists a element with the same name")) };
	}

	Result UIScene::Load(const Path& path)
	{
		UISceneParser parser;
		return parser.Parse(this, &resourceStorage, parsingData, path);
	}

	UIBaseElementManager* UIScene::GetElementManager(uint type)
	{
		return managers[type];
	}

	Result UIScene::SetupElementManagers()
	{
		return manager.SetupElementManagers();
	}
	Result UIScene::CleanupElementManagers()
	{
		return manager.CleanupElementManagers();
	}

	Result UIScene::Render()
	{
		return manager.Render();
	}
	Result UIScene::Update()
	{
		return manager.Update();
	}
	inline uint UIScene::GetLayerCount() const
	{
		return layers.size();
	}
	UIScene::EventFunction UIScene::GetEventFunction(StringView name) const
	{
		auto it = functions.find(name);
		if (it == functions.end())
			return EventFunction();
		return it->second;
	}
	std::pair<UIElement*, uint> UIScene::GetElement(StringView name) const
	{
		auto it = elementNames.find((String)name);
		if (elementNames.end() == it)
			return { nullptr, -1 };
		return 
			it->second;
	}
}
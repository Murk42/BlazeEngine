#include "BlazeEngine/Application/UI System/UIScene.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Core/MemoryManager.h"

#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"


#include "UISceneParser.h"

namespace Blaze
{
	namespace UI
	{							

		UIScene::UIScene()
			: typeCount(0), managers(nullptr)
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
					const ElementTypeData& typeData = manager.GetElementTypeRegistry().GetElementTypeData(i);
					for (auto& element : layer.arrays[i].elements)
					{
						typeData.destruct(element);
						Memory::Free(element);
					}
				}

				delete[] layer.arrays;
			}
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

			for (int i = 0; i < typeCount; ++i)
			{
				const auto& typeData = manager.GetElementTypeRegistry().GetElementTypeData(i);
				typeData.destructManager(managers[i]);
				Memory::Free(managers[i]);
			}
			delete[] managers;

			typeCount = registry.GetElementTypeCount();
			manager.SetElementTypeRegistry(registry);

			managers = new UIBaseElementManager*[typeCount];

			for (int i = 0; i < typeCount; ++i)
			{
				const auto& typeData = registry.GetElementTypeData(i);
				managers[i] = (UIBaseElementManager*)Memory::Allocate(typeData.managerSize);
				typeData.constructManager(managers[i]);

				manager.SetElementManager(managers[i], i);
			}
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
		void UIScene::AddEventFunction(StringView name, EventFunction func)
		{
			functions.insert({ name, func });
		}
		UIElement* UIScene::CreateElement(uint typeIndex, uint layer)
		{
			const ElementTypeData& typeData = manager.GetElementTypeRegistry().GetElementTypeData(typeIndex);
			
			auto& elements = layers[layer].arrays[typeIndex].elements;
			void*& element = elements.emplace_back();
			element = Memory::Allocate(typeData.size);
			typeData.construct(element);

			manager.AddElement((UIElement*)element, typeIndex, layer);
			return (UIElement*)element;
		}
		UIElement* UIScene::CreateElement(StringView name, uint typeIndex, uint layer)
		{
			UIElement* element = CreateElement(typeIndex, layer);
			elementNames[name] = element;
			return element;
		}

		Result UIScene::Load(const Path& path)
		{			
			UISceneParser parser;						
			return parser.Parse(this, &resourceStorage, path);			
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
		UIScene::EventFunction UIScene::GetEventFunction(StringView name)
		{
			auto it = functions.find(name);
			if (it == functions.end())
				return EventFunction();
			return it->second;
		}
		UIElement* UIScene::GetElement(StringView name)
		{
			return elementNames[name];
		}
	}
}
#pragma once
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/UI System/UIElement.h"

namespace Blaze::UI
{	
	class UISceneData;

	class BLAZE_API UIScene : public ResourceSystem::Resource
	{
	public:
		using EventFunction = std::function<void()>;
	private:
		UIManager* manager;

		ResourceSystem::ResourceStorage resourceStorage;

		std::vector<UIElement*> elements;
		std::unordered_map<String, UIElement*> nameToElementMap;
		std::unordered_map<String, EventFunction> functions;
	public:
		UIScene();
		~UIScene();

		Result SetManager(UIManager* manager);
		void SetResourceManager(ResourceSystem::ResourceManager* resourceManager);
		
		void SetEventFunction(StringView name, const EventFunction& function);

		template<typename T>
		ResultValue<T*> CreateElement(StringView layer)
		{
			ResultValue<UIElement*> res = CreateElement(manager->GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			return ResultValue<T*>((T*)res.value, std::move(res.result));
		}
		ResultValue<UIElement*> CreateElement(uint typeIndex, StringView layer);
		template<typename T>
		ResultValue<T*> CreateElement(StringView name, StringView layer)
		{
			ResultValue<UIElement*> res = CreateElement(name, manager->GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			return ResultValue<T*>((T*)res.value, std::move(res.result));
		}
		ResultValue<UIElement*> CreateElement(StringView name, uint typeIndex, StringView layer);
		
		Result DestroyElement(UIElement* element);

		void RenameElement(UIElement* element, StringView newName);

		template<typename T>
		ResultValue<T*> GetElement(StringView name) const
		{
			auto value = GetElementBase(name);

			if (value != nullptr && value->typeIndex == manager->GetElementTypeRegistry().GetElementTypeIndex<T>())
				return { (T*)value, Result() };
			else
				return { nullptr, Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"BlazeEngine", "Element name and type arent matching")) };
		}
		UIElement* GetElementBase(StringView name) const;

		Result Load(const Path& path);

		EventFunction GetEventFunction(StringView name) const;

		bool IsAvailableName(String name);

		inline auto& GetResourceStorage() { return resourceStorage; }
		inline const auto& GetResourceStorage() const { return resourceStorage; }
		inline auto* GetResourceManager() const { return resourceStorage.GetResourceManager(); }
		inline auto* GetManager() const { return manager; }

		static constexpr const char* typeName = "UIScene";

		friend class UIElement;
	};
}

/*
SceneData
	elementType
	elementData
*/
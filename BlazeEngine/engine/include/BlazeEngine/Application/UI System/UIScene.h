#pragma once
#include "BlazeEngine/Core/ResultValue.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/UI System/UIElementParsingData.h"

namespace Blaze
{
	namespace UI
	{		
		class BLAZE_API UIScene
		{		
		public:
			using EventFunction = std::function<void()>;
		private:
			struct TypeStorage
			{
				std::vector<void*> elements;
			};			
			struct LayerStorage
			{				
				TypeStorage* arrays;
			};			
			
			size_t typeCount;
			UIManager manager;
			
			Resource::ResourceStorage resourceStorage;
			UIElementParsingData* parsingData;
		
			UIBaseElementManager** managers;
			std::vector<LayerStorage> layers;
			std::unordered_map<String, EventFunction> functions;
			std::unordered_map<String, std::pair<UIElement*, uint>> elementNames;

			Result SetupElementManagers();
			Result CleanupElementManagers();
		public:
			UIScene();
			~UIScene();

			void SetElementTypeRegistry(const UIElementTypeRegistry& registry);
			void SetResourceManager(Resource::ResourceManager* resourceManager);

			template<typename T>
			void AddElementParsingData()
			{
				AddElementParsingData(T::ManagerType::GetElementParsingData(), manager.GetElementTypeRegistry().GetElementTypeIndex<T>());
			}
			void AddElementParsingData(const UIElementParsingData& parsingData, uint type);
			void AddElementParsingData(UIElementParsingData&& parsingData, uint type);

			void AddCoreElementsParsingData();

			void AddEventFunction(StringView name, EventFunction function);

			template<typename T>
			typename T::ManagerType* GetElementManager()
			{
				return (typename T::ManagerType*)GetManager(manager.GetElementTypeRegistry().GetElementTypeIndex<T>());
			}
			UIBaseElementManager* GetElementManager(uint type);			

			template<typename T>
			ResultValue<T*> CreateElement(uint layer)
			{
				return (T*)CreateElement(
					manager.GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			}
			ResultValue<UIElement*> CreateElement(uint typeIndex, uint layer);
			template<typename T>
			ResultValue<T*> CreateElement(StringView name, uint layer)
			{
				return (T*)CreateElement(name, manager.GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			}
			ResultValue<UIElement*> CreateElement(StringView name, uint typeIndex, uint layer);

			uint CreateLayer();

			Result Load(const Path& path);

			Result Render();
			Result Update();

			inline bool TakeFocus() { return manager.TakeFocus(); }
			inline void ReturnFocus() { manager.ReturnFocus(); }
			inline bool IsFocusTaken() const { return manager.IsFocusTaken(); }
			inline uint GetTypeCount() const { return typeCount; }
			inline uint GetLayerCount() const;			
			inline Resource::ResourceManager* GetResourceManager() const { return resourceStorage.GetResourceManager(); }
			inline EventFunction GetEventFunction(StringView name) const;
			inline std::pair<UIElement*, uint> GetElement(StringView name) const;						

			const UIManager& GetManager() const { return manager; }
		};
	}
}
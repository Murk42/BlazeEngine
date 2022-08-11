#pragma once
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"

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
		
			UIBaseElementManager** managers;
			std::vector<LayerStorage> layers;				
			std::unordered_map<String, EventFunction> functions;
			std::unordered_map<String, UIElement*> elementNames;
		public:	

			UIScene();
			~UIScene();

			void SetElementTypeRegistry(const UIElementTypeRegistry& registry);			
			void SetResourceManager(Resource::ResourceManager* resourceManager);

			uint CreateLayer();

			template<typename T>
			T* CreateElement(uint layer)
			{
				return (T*)CreateElement(
					manager.GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			}
			UIElement* CreateElement(uint typeIndex, uint layer);
			template<typename T>
			T* CreateElement(StringView name, uint layer)
			{
				return (T*)CreateElement(name, manager.GetElementTypeRegistry().GetElementTypeIndex<T>(), layer);
			}
			UIElement* CreateElement(StringView name, uint typeIndex, uint layer);

			void AddEventFunction(StringView name, EventFunction function);

			Result Load(const Path& path);

			template<typename T>
			typename T::ManagerType * GetManager()
			{
				return (typename T::ManagerType*)managers[manager.GetElementTypeRegistry().GetElementTypeIndex<T>()];
			}

			Result SetupElementManagers();
			Result CleanupElementManagers();

			Result Render();
			Result Update();

			bool TakeFocus() { return manager.TakeFocus(); }
			void ReturnFocus() { manager.ReturnFocus(); }
			bool IsFocusTaken() const { return manager.IsFocusTaken(); }
			
			EventFunction GetEventFunction(StringView name);
			UIElement* GetElement(StringView name);

			const UIManager& GetManager() const { return manager; }
		};
	}
}
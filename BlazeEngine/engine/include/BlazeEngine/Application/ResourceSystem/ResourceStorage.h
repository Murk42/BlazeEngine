#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/ResultValue.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"

namespace Blaze::Resource
{
	class ResourceManager;

	class BLAZE_API ResourceStorage
	{
		ResourceTypeRegistry registry;
		size_t typeCount;
		
		ResourceManager* manager;

		std::list<void*>* resources;
	public:
		ResourceStorage();
		~ResourceStorage();

		void SetResourceTypeRegistry(const ResourceTypeRegistry&);
		void SetResourceManager(ResourceManager* manager);

		template<typename T>
		ResultValue<T*> CreateResource()
		{
			int typeIndex = registry.GetTypeIndex<T>();
			if (typeIndex == -1)
			{				
				return { nullptr, Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Creating resource of an unregistered type")) };
			}
			else
				return (T*)CreateResource(registry.GetTypeIndex<T>());
		}
		template<typename T>
		ResultValue<T*> CreateResource(StringView name)
		{
			int typeIndex = registry.GetTypeIndex<T>();
			if (typeIndex == -1)
			{
				return { nullptr, Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Creating resource of an unregistered type")) };
			}
			else
				return (T*)CreateResource(name, registry.GetTypeIndex<T>());
		}
		void* CreateResource(size_t typeIndex);
		void* CreateResource(StringView name, size_t typeIndex);

		inline const ResourceTypeRegistry& GetResourceTypeRegistry() const { return registry; }
		inline ResourceManager* GetResourceManager() const { return manager; }
	};
}
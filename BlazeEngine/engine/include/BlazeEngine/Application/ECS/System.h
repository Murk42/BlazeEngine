#pragma once
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Component;
	class ComponentContainer;
	class Manager;

	class BLAZE_API System
	{
		uint typeIndex;
		Manager* manager;

		const ComponentTypeRegistry& GetRegistry() const;
	public:
		virtual void Created(Component*);
		virtual void Destroyed(Component*);

		virtual void Update(const ComponentContainer&);
		virtual void Render(const ComponentContainer&);

		virtual bool SetProperty(Component*, StringView name, StringView value);		

		inline Manager* GetManager() const { return manager; }
		inline uint GetTypeIndex() const { return typeIndex; }

		template<typename T>
		T::System* Cast()
		{
			if (typeIndex != GetRegistry().GetComponentTypeIndex<T>())
			{
				Logger::AddLog(BLAZE_FATAL_LOG("Blaze Engine", "Invalid component cast"));
				return nullptr;
			}

			return (typename T::System*)this;
		}

		friend class Manager;
	};
}
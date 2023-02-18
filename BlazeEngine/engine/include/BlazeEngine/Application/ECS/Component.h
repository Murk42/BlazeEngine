#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "ComponentTypeRegistry.h"
#include "System.h"

namespace Blaze::ECS
{
	class Entity;

#define COMPONENT(name, systemName)						\
	static constexpr const char* typeName = #name;		\
	using System = systemName;

	class BLAZE_API Component
	{
		uint bucketIndex;
		uint typeIndex;
		Entity* entity;

		const ComponentTypeRegistry& GetRegistry() const;
	public:
		inline uint GetTypeIndex() const { return typeIndex; }
		inline Entity* GetEntity() const { return entity; }

		template<typename T>
		T* Cast()
		{
			if (typeIndex != GetRegistry().GetComponentTypeIndex<T>())
			{				
				Logger::AddLog(BLAZE_ERROR_LOG("Blaze Engine", "Invalid component cast"));
				return nullptr;
			}
		
			return (T*)this;
		}

		friend class ComponentContainer;
		friend class Scene;
	};

	class BLAZE_API ComponentContainer
	{
		ComponentTypeData typeData;
		static constexpr size_t bucketElementCount = 16;

		struct BucketHeader
		{
			uint16 flags;
			byte data[];
		};
		std::vector<BucketHeader*> buckets;
		size_t count;
	public:
		class BLAZE_API Iterator
		{
			const ComponentContainer* container;
			uint bucketIndex;
			uint index;

			Iterator(const ComponentContainer*, uint, uint);
		public:
			Iterator(const Iterator&);

			Iterator operator++();
			Iterator& operator++(int);
			Iterator operator--();
			Iterator& operator--(int);
			
			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

			Component* operator*();
			Component* operator->();

			Iterator& operator=(const Iterator&);

			friend class ComponentContainer;
		};

		ComponentContainer();
		~ComponentContainer();

		Result SetTypeData(const ComponentTypeData&);

		void Clear();

		Component* Create();
		Result Destroy(Component*);

		Iterator begin() const;
		Iterator end() const;

		friend class Iterator;
	};

	class BLAZE_API ComponentGroup
	{
		size_t stateSize;
		size_t count;
		void* data;
	public:
		ComponentGroup();
		~ComponentGroup();

		Result Resize(std::initializer_list<uint> typeIndicies, size_t typeCount);
		void Clear();

		Result SetComponent(Component* component);

		size_t Count() const;

		template<typename T>
		bool HasComponent() const 
		{
			return HasComponent(T::typeIndex);
		}
		bool HasComponent(uint typeIndex) const;
		template<typename T>
		T* GetComponent() const
		{
			return GetComponent(T::typeIndex);
		}
		Component* GetComponent(uint typeIndex) const;

		Component** begin() const;
		Component** end() const;
	};
}
#pragma once
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

		static constexpr size_t BucketElementCount = 32;
		using FlagType =
			std::conditional_t<BucketElementCount == 8, uint8,
			std::conditional_t<BucketElementCount == 16, uint16,
			std::conditional_t<BucketElementCount == 32, uint32,
			std::conditional_t<BucketElementCount == 64, uint64,
			void>>>>;

		struct BucketHeader;

		struct ElementHeader
		{
			BucketHeader* bucket;			
		};

		struct BucketHeader
		{
			FlagType flags;			
		};

		uint elementSize;
		uint elementCount;
		BucketHeader** buckets;
		uint bucketCount;
		uint nonFullBucketCount;		

		ElementHeader* BucketAllocate(BucketHeader* bucket);
		void BucketFree(ElementHeader* ptr);
		void* FirstInBucket(BucketHeader* bucket) const;
		void* LastInBucket(BucketHeader* bucket) const;

		void Increment(uint& bucketIndex, Component*& ptr) const;
		void Decrement(uint& bucketIndex, Component*& ptr) const;
	public:
		ComponentContainer();
		~ComponentContainer();

		Result SetTypeData(const ComponentTypeData&);

		Component* Create();
		void Destroy(Component*);

		uint Count() const { return elementCount; }

		void Clear();

		class BLAZE_API Iterator
		{
			const ComponentContainer* container;
			uint bucketIndex;
			Component* ptr;

			Iterator(const ComponentContainer*, uint, Component*);
		public:
			Iterator();
			Iterator(const Iterator&);

			Iterator& operator++();
			Iterator operator++(int);
			Iterator& operator--();
			Iterator operator--(int);
			
			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

			Component* operator*();
			Component* operator->();

			Iterator& operator=(const Iterator&);

			friend class ComponentContainer;
		};

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
#pragma once
#include "ComponentTypeRegistry.h"
#include "System.h"
#include "BlazeEngine/DataStructures/Constexpr.h"

namespace Blaze::ECS
{			
	template<Constexpr::FixedString ... Tags>
	struct TagGroup
	{
		static constexpr uintMem Size = 0;
		static constexpr std::array<StringView, 0> tags;

		template<Constexpr::FixedString ... NewTags>
		using Concat = TagGroup<NewTags...>;
		template<typename G>
		using Combine = G;
	};			
	template<Constexpr::FixedString Tag, Constexpr::FixedString Tag2, Constexpr::FixedString ... Tags>
	struct TagGroup<Tag, Tag2, Tags...>
	{
		static constexpr uintMem Size = 2 + sizeof...(Tags);

		template<Constexpr::FixedString ... NewTags>
		using Concat = TagGroup<Tag, Tag2, Tags..., NewTags...>;
		template<typename G>
		using Combine = typename G::template Concat<Tag, Tag2, Tags...>;
		using Next = TagGroup<Tag2, Tags...>;

		static constexpr StringView tag = Tag;
		static constexpr std::array<StringView, Size> tags = Constexpr::ConcatArray(std::array<StringView, 1>({ Tag }), Next::tags);
	};
	template<Constexpr::FixedString Tag>
	struct TagGroup<Tag>
	{	
		static constexpr uintMem Size = 1;

		template<Constexpr::FixedString ... NewTags>
		using Concat = TagGroup<Tag, NewTags...>;
		template<typename G>
		using Combine = typename G::template Concat<Tag>;

		using Next = TagGroup<>;

		static constexpr StringView tag = Tag;
		static constexpr std::array<StringView, 1> tags{ tag };
	};

	template<typename T>
	concept HasValidTypeTags = requires {
		typename T::TypeTags;
	};
	template<typename T>
	concept DerivesComponent = requires {
		std::derived_from<T, Component>;
	};

	class Entity;	

#define COMPONENT(name, systemName)											\
	static constexpr StringView typeName = #name;							\
	using System = systemName;												\
	static_assert(::Blaze::ECS::HasValidTypeTags<name>, "The component must have a valid \"TypeTags\" typename for it to be a valid component. Maybe multiple base classes have the typename already defined, use \"COMPONENT_SET_TYPE_TAGS\" or \"COMPONENT_COMBINE_TYPE_TAGS\" in that case");\

#define COMPONENT_ADD_TYPE_TAGS(...) using TypeTags = TypeTags::Concat<__VA_ARGS__>;

	class BLAZE_API Component
	{				
		Entity* entity;
		System* system;
	public:
		Component();	
		Component(const Component&) = delete;
		
		bool GetTypeData(const ComponentTypeData*& typeData) const;
		inline System* GetSystem() const { return system; }
		inline Entity* GetEntity() const { return entity; }		

		template<typename T>
		inline T* Cast()
		{
			const ComponentTypeData* typeData;
			if (!GetTypeData(typeData))
				Debug::Logger::LogError("Blaze Engine", "Component has to type data");

			if (typeData->GetTypeName() != T::typeName)				
				Debug::Logger::LogError("Blaze Engine", "Invalid component cast");								

			return (T*)this;					
		}

		Component& operator=(const Component&) = delete;

		using TypeTags = TagGroup<>;

		friend class ComponentContainer;
		friend class Scene;
	};

	class BLAZE_API ComponentContainer
	{
		const ComponentTypeData* typeData;

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
		
			uintMem MarkNew();
			void Unmark(uintMem index);

			bool IsFull();			
			bool IsEmpty();
		};

		//Important for bucket storage
		uintMem elementSize;
		BucketHeader** buckets;
		uintMem bucketCount;

		//Important for bucket manipulation
		uintMem nonFullBucketCount;

		//Important for client
		uintMem elementCount;

		void GetComponentLocation(Component*, BucketHeader*&, uintMem&);
		Component* GetComponentFromLocation(BucketHeader*, uintMem index, ElementHeader*& element);

		void* FirstInBucket(BucketHeader* bucket) const;
		void* LastInBucket(BucketHeader* bucket) const;

		void Increment(uintMem& bucketIndex, Component*& ptr) const;
		void Decrement(uintMem& bucketIndex, Component*& ptr) const;

		BucketHeader* AllocateBucket();
		void FreeBucket(BucketHeader* bucket);

		void RemoveBucket(BucketHeader* bucket);
		void AddBucket(BucketHeader* bucket);
	public:
		ComponentContainer();
		~ComponentContainer();

		Result SetTypeData(const ComponentTypeData&);

		Component* Create();
		Component* Allocate();
		void Destroy(Component*);
		void Free(Component*);

		uintMem Count() const { return elementCount; }

		void Clear();

		class BLAZE_API Iterator
		{
			const ComponentContainer* container;
			uintMem bucketIndex;
			Component* ptr;

			Iterator(const ComponentContainer*, uintMem, Component*);
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
}
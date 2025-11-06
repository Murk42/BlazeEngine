#pragma once
#include "BlazeEngineCore/String/FixedString.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Application/ECS/System.h"

namespace Blaze::ECS
{
	template<FixedString ... Tags>
	struct TagGroup
	{
		static constexpr uintMem Size = 0;
		static constexpr std::array<StringView, 0> tags;

		template<FixedString ... NewTags>
		using Concat = TagGroup<NewTags...>;
		template<typename G>
		using Combine = G;
	};
	template<FixedString Tag, FixedString Tag2, FixedString ... Tags>
	struct TagGroup<Tag, Tag2, Tags...>
	{
		static constexpr uintMem Size = 2 + sizeof...(Tags);

		template<FixedString ... NewTags>
		using Concat = TagGroup<Tag, Tag2, Tags..., NewTags...>;
		template<typename G>
		using Combine = typename G::template Concat<Tag, Tag2, Tags...>;
		using Next = TagGroup<Tag2, Tags...>;

		static constexpr StringView tag = Tag;
		static constexpr std::array<StringView, Size> tags = ConcatArray(std::array<StringView, 1>({ Tag }), Next::tags);
	};
	template<FixedString Tag>
	struct TagGroup<Tag>
	{
		static constexpr uintMem Size = 1;

		template<FixedString ... NewTags>
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
		IsDerivedFrom<T, Component>;
	};

	class Entity;

#define COMPONENT(name, systemName)											\
	static constexpr StringView typeName = #name;							\
	using System = systemName;

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
				BLAZE_LOG_ERROR("Component has to type data");

			if (typeData->GetTypeName() != T::typeName)
				BLAZE_LOG_ERROR("Invalid component cast");

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

		void SetTypeData(const ComponentTypeData&);

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
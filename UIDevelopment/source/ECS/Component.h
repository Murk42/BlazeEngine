#pragma once
#include "ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Component
	{
		uint bucketIndex;
		uint typeIndex;
	public:

		friend class ComponentContainer;
		friend class Scene;
	};

	class ComponentContainer
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
		ComponentContainer();
		~ComponentContainer();

		Result SetTypeData(const ComponentTypeData&);

		void Clear();

		Component* Create();
		Result Destroy(Component*);
	};
}
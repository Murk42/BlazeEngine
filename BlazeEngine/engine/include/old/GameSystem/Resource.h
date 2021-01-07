#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/GameSystem/Common.h"
#include <vector>

namespace Blaze
{	
	class BaseScene;
	class ByteStream;
	
	typedef uint32 ResourceTypeID;
	
	class BLAZE_API BaseResource
	{
		template<typename ... T>
		using Func = void(*)(void*, T...);

		BaseScene* scene;
		ResourceID id;

		BaseResource(ResourceTypeID type);

		static ResourceTypeID RegisterResourceType(Func<>, Func<>, size_t, size_t, Func<ByteStream&>, Func<ByteStream&>);
	public:
		const ResourceTypeID typeID;

		inline BaseScene* GetScene() const { return scene; }
		inline ResourceID GetID() const { return id; }

		void Serialize(ByteStream&) {}
		void Deserialize(ByteStream&) {}

		template<typename>
		friend class Resource;
		friend class BaseScene;

	};

	template<typename Derived>
	class Resource : public BaseResource
	{
		static void _Construct(void* ptr) { new (ptr) Derived(); }
		static void _Destruct(void* ptr) { ((Derived*)ptr)->~Derived(); }		
		static void _Serialize(void* ptr, ByteStream& bs) { ((Derived*)ptr)->Serialize(bs); }		
		static void _Deserialize(void* ptr, ByteStream& bs) { ((Derived*)ptr)->Deserialize(bs); }

		struct Init
		{
			ResourceTypeID typeID;
			Init() : typeID(BaseResource::RegisterResourceType(_Construct, _Destruct, sizeof(Derived), baseoffset(Derived, BaseResource), _Serialize, _Deserialize)) { }
		} static init;
	public:						
		Resource() : BaseResource(init.typeID) {}

		static ResourceTypeID GetTypeID() { return init.typeID; }
	};
}
#pragma once
#include <cstdlib>
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{

	template<typename Base>
	struct BaseType
	{
		void (*construct)(void*);
		void (*destruct)(void*);
		size_t size;
		size_t baseOffset;

		void* Allocate() const
		{
			return new uint8[size];
		}
		void Deallocate(void* ptr) const
		{
			delete[] (uint8*)ptr;
		}
		Base* ToBase(void* ptr) const
		{
			return (Base*)((uint8*)ptr + baseOffset);
		}
		const Base* ToBase(const void* ptr) const
		{
			return (const Base*)((const uint8*)ptr + baseOffset);
		}
		void* ToDerived(Base* base) const
		{
			return (void*)((uint8*)base - baseOffset);
		}
		const void* ToDerived(const Base* base) const
		{
			return (const void*)((const uint8*)base - baseOffset);
		}
	};


	class BaseComponent;	
	struct ComponentType : BaseType<BaseComponent>
	{		
		size_t hash;
		void (*copy)(void*, const void*);	
		std::vector<std::pair<size_t, size_t>> bases;
	};


	class BaseSystem;
	struct SystemType : BaseType<BaseSystem>
	{		
		void (*update)(void*);		
	};


	class BaseScene;
	struct SceneType : BaseType<BaseScene>
	{		
		void (*load)(void*);
		void (*unload)(void*);
		void (*save)(void*);
		void (*start)(void*);
		void (*update)(void*, float);
		void (*stop)(void*);		
	};


	class BaseResource;
	class ByteStream;
	struct ResourceType : BaseType<BaseResource>
	{		
		void(*serialize)(void*, ByteStream&);		
		void(*deserialize)(void*, ByteStream&);		
	};
}
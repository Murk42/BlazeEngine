#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Common.h"
#include "BlazeEngine/GameSystem/ECS/ECS.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include <tuple>
#include <typeinfo>

#include "BlazeEngine/DataStructures/TypeTraits.h"

namespace Blaze
{
	class ECS;
	 
	class BLAZE_API BaseSystem
	{	
		struct TypeData
		{		
			ComponentTypeID typeID;
			size_t size;
			size_t offset;
			uint8* begin;
			uint8* end;
		};

		template<typename ... T>
		using Func = void(*)(void*, T...);

		ECS* ecs;

		BaseSystem(SystemTypeID);

		static SystemTypeID RegisterSystemType(Func<>, Func<>, size_t, size_t, Func<>);
		inline void InitializeTypes(std::vector<TypeData>&, size_t, bool);
		inline void UpdateTypes(std::vector<TypeData>&);
	protected:		
		template<typename>
		struct Array;		
	public:
		const SystemTypeID typeID;

		template<typename, typename ...>
		friend class System;
		friend class ECS;
	};	

	template<typename T>
	struct BaseSystem::Array
	{		
	private:
		std::vector<BaseSystem::TypeData> types;
	public:
		struct Iterator
		{
		private:
			const std::vector<BaseSystem::TypeData>& types;			
			uint type;
			uint8* ptr;

			inline void Increase(uint amount)
			{		
				if (types.size() == 0)
					return;

				while (ptr + amount * types[type].size >= types[type].end && type != types.size() - 1)
				{									
					amount -= (ptr - types[type].end) / (int)types[type].size + amount + 1;
					while (type != types.size() - 1 && types[type].begin != nullptr)
					{
						++type;
						ptr = types[type].begin;
					}
				}
				
				if (ptr != nullptr || type == types.size() + 1)
					ptr += amount * types[type].size;
				else
					ptr = nullptr;
			}
			inline void Decrease(uint amount)
			{
				if (types.size() == 0)
					return;

				while (ptr - amount * types[type].size < types[type].begin && type != 0)
				{
					amount -= (types[type].begin - ptr) / (int)types[type].size + amount + 1;
					while (type != 0 && types[type].end != nullptr)
					{
						--type;
						ptr = types[type].end;
					}
				}

				if (ptr != nullptr || type == types.size() + 1)
					ptr -= amount * types[type].size;
				else
					ptr = nullptr;
			}			

			Iterator(uint8* ptr, uint type, const std::vector<BaseSystem::TypeData>& types) : ptr(ptr), type(type), types(types) { }
		public:
			Iterator(const Iterator& i) : ptr(i.ptr), type(i.type), types(i.types) { }

			inline BaseComponent& GetBaseComponent()
			{
				return *(BaseComponent*)ptr;
			}
			inline T& Get()
			{
				return *(T*)(ptr + types[type].offset);
			}

			inline T& operator*()
			{
				return Get();
			}
			inline T* operator->()
			{
				return &Get();
			}

			Iterator& operator+= (size_t offset)
			{
				Increase(offset);				
				return *this;
			}
			Iterator operator+(size_t offset) const
			{
				Iterator it(*this);
				it.Increase(offset);
				return it;
			}
			Iterator& operator++()
			{
				Increase(1);				
				return *this;
			}
			Iterator& operator++(int)
			{
				Iterator it(*this);
				it.Increase(1);
				return it;
			}

			Iterator& operator-= (size_t offset)
			{
				Decrease(offset);
				return *this;
			}
			Iterator operator-(size_t offset) const
			{
				Iterator it(*this);
				it.Decrease(offset);
				return it;
			}
			Iterator& operator--()
			{
				Decrease(1);
				return *this;
			}
			Iterator& operator--(int)
			{
				Iterator it(*this);
				it.Decrease(1);
				return *this;
			}

			bool operator==(const Iterator& i) const
			{
				return ptr == i.ptr;
			}
			bool operator!=(const Iterator& i) const
			{
				return ptr != i.ptr;
			}

			bool operator>(const Iterator& i) const
			{
				return ptr > i.ptr;
			}
			bool operator>=(const Iterator& i) const
			{
				return ptr >= i.ptr;
			}

			bool operator<(const Iterator& i) const
			{
				return ptr < i.ptr;
			}
			bool operator<=(const Iterator& i) const
			{
				return ptr <= i.ptr;
			}
			
			template<typename>
			friend class Array;
		};

		Array()
		{

		}
	
		size_t Size() const
		{
			size_t sum = 0;
			for (const auto& t : types)
				sum += (t.end - t.begin) / t.size;
			return;
		}
	
		Iterator begin() const
		{					
			if (types.size() > 0)
				return Iterator(types[0].begin, 0, types);
			else
				return Iterator(nullptr, 0, types);
		}
		Iterator end() const
		{
			if (types.size() > 0)
				return Iterator(types.back().end, types.size() - 1, types);
			else
				return Iterator(nullptr, 0, types);
		}

		T& operator[](uint i)
		{
			Iterator it = begin();
			it.Increase(i);
			return *it;
		}
		
		template<typename, typename ...>
		friend class System;		
	};

	template<typename Derived, typename ... T>
	class System : public BaseSystem
	{		
		std::tuple<Array<T>...> arrays;

		static void Construct(void* system) { new (system) Derived(); }
		static void Destruct(void* system) { ((Derived*)system)->~Derived(); }
		static void Update(void* system);

		struct Init
		{
			SystemTypeID typeID;
			Init()
				: typeID(BaseSystem::RegisterSystemType(Construct, Destruct, sizeof(Derived), baseoffset(Derived, BaseSystem), Update))
			{

			}
		} static init;	
	public:
		System();

		inline static SystemTypeID GetTypeID() { return init.typeID; }
	};

	template<typename Derived, typename ... T>
	inline System<Derived, T...>::System() : BaseSystem(typeID)
	{				
		(InitializeTypes(std::get<index_of_v<T, T...>>(arrays).types, typeid(T).hash_code(), std::is_base_of_v<BaseComponent, T>), ...);
	}

	template<typename Derived, typename ... T>
	inline void System<Derived, T...>::Update(void* ptr)
	{	 
		Derived* system = (Derived*)ptr;
		System* base = system;
		(base->UpdateTypes(std::get<index_of_v<T, T...>>(base->arrays).types), ... );

		system->Update(std::get<index_of_v<T, T...>>(base->arrays)...);
	}
}

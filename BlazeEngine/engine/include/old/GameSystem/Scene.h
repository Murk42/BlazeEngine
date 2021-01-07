#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/GameSystem/ECS/ECS.h"
#include "BlazeEngine/GameSystem/Common.h"
#include "BlazeEngine/GameSystem/Resource.h"
#include <unordered_map>
#include <tuple>

namespace Blaze
{	
	typedef uint32 SceneTypeID;
	
	class BLAZE_API BaseScene	
	{										
		template<typename ... T>
		using Func = void(*)(void*, T...);		

		ECS ecs;						
		
		std::vector<ResourceID> resources;

		BaseScene(SceneTypeID);
		~BaseScene();
		
		static SceneTypeID RegisterSceneType(Func<>, Func<>, size_t, size_t, Func<>, Func<>, Func<>, Func<>, Func<float>, Func<>);
	public:			
		const SceneTypeID typeID;		

		inline Entity CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& list);
		inline Entity CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& list, const std::initializer_list<const BaseComponent*>& components);
		template<typename ... T>
		inline Entity CreateEntity(String&& name)
		{
			return ecs.CreateEntity<T...>(std::move(name));
		}
		template<typename ... T>
		inline Entity CreateEntity(String&& name, const T& ... components)
		{
			return ecs.CreateEntity<T...>(std::move(name), components...);
		}		
		inline void DeleteEntity(const Entity&);

		inline void UpdateSystem(SystemTypeID id)
		{
			ecs.UpdateSystem(id);
		}
		template<typename T>
		inline void UpdateSystem()
		{
			ecs.UpdateSystem<T>();
		}
		inline BaseSystem* GetSystem(SystemTypeID id)
		{
			return ecs.GetSystem(id);
		}
		template<typename T>
		inline T* GetSystem()
		{
			return ecs.GetSystem(T::GetTypeID());
		}
		
		void Load() { }
		void Unload() { }
		void Save() { }
		void Start() {}
		void Stop() {}					
		void Update(float dt) {}
				
		template<typename>
		friend class Scene;
		friend class ECS;
	};

	template<typename Derived>
	class Scene : public BaseScene
	{
		inline static void Construct(void* ptr) { new (ptr) Derived(); }
		inline static void Destruct(void* ptr) { ((Derived*)ptr)->~Derived(); }

		inline static void _Load(void* ptr) { ((Derived*)ptr)->Load(); }
		inline static void _Unload(void* ptr) { ((Derived*)ptr)->Unload(); }
		inline static void _Save(void* ptr) { ((Derived*)ptr)->Save(); }

		inline static void _Start(void* ptr) { ((Derived*)ptr)->Start(); }
		inline static void _Update(void* ptr, float dt) { ((Derived*)ptr)->Update(dt); }
		inline static void _Stop(void* ptr) { ((Derived*)ptr)->Stop(); }

		struct Init
		{
			SceneTypeID typeID;
			Init()
				: typeID(BaseScene::RegisterSceneType(Construct, Destruct, sizeof(Derived), baseoffset(Derived, BaseScene), _Load, _Unload, _Save, _Start, _Update, _Stop))
			{

			}
		} static init;
	public:			
		Scene() : BaseScene(init.typeID) { }	
		~Scene() = default;

		static inline SceneTypeID GetTypeID() { return init.typeID; }
	};

	template<typename T>
	Scene<T>::template Init Scene<T>::init;
}
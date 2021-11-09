#pragma once
#include "BlazeEngine/Core/EngineCore.h"

namespace Blaze
{
	struct SceneInfo
	{
		using Constructor = void(*)(void*);
		using Destructor = void(*)(void*);

		Constructor constructor = nullptr;
		Destructor destructor = nullptr;
		size_t size = 0;
	};

	class SceneBase
	{
	public:
		virtual void Startup() { };
		virtual void Update() { };
		virtual void Cleanup() { };
	};

	template<typename T>
	class Scene : public SceneBase
	{		
		static void Constructor(void* ptr) { new (ptr) T(); }
		static void Destructor(void* ptr) { ((T*)ptr)->~T(); }
	public:		
		static SceneInfo GetSceneInfo()
		{
			return { Constructor, Destructor, sizeof(T) };
		}
	};	

	BLAZE_API void StartBlazeApplication(SceneInfo sceneInfo);

	template<typename T>
	class StartupScene : public Scene<T>
	{		
		struct Init{
			Init()
			{
				StartBlazeApplication(Scene<T>::GetSceneInfo())	;
			}
		} static inline init;
	public:		
	};
}
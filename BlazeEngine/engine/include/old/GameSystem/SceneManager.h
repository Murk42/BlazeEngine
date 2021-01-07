#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/Scene.h"
#include <vector>

namespace Blaze
{				
	namespace SceneManager
	{			
		void BLAZE_API LoadScene(SceneTypeID);
		template<typename T> 
		inline void LoadScene()
		{
			LoadScene(T::GetTypeID());
		}		

		void BLAZE_API LoadSceneAsync(SceneTypeID);
		template<typename T>
		inline void LoadSceneAsync()
		{
			LoadSceneAsync(T::GetTypeID());
		}

		void BLAZE_API UnloadScene(SceneTypeID);
		template<typename T>
		inline void UnloadScene()
		{
			UnloadScene(T::GetTypeID());
		}

		void BLAZE_API UnloadSceneAsync(SceneTypeID);
		template<typename T>
		inline void UnloadSceneAsync()
		{
			UnloadScene(T::GetTypeID());
		}

		void BLAZE_API SaveScene(SceneTypeID);
		template<typename T>
		inline void SaveScene()
		{
			SaveScene(T::GetTypeID());
		}

		void BLAZE_API SaveSceneAsync(SceneTypeID);
		template<typename T>
		inline void SaveSceneAsync()
		{
			SaveSceneAsync(T::GetTypeID());
		}

		bool BLAZE_API IsLoaded(SceneTypeID);
		template<typename T>
		inline bool IsLoaded()
		{
			return IsLoaded(T::GetTypeID());
		}

		bool BLAZE_API IsLoading(SceneTypeID);
		template<typename T>
		inline bool IsLoading()
		{
			return IsLoading(T::GetTypeID());
		}
		
		void BLAZE_API RunScene(SceneTypeID);
		template<typename T>
		inline void RunScene()
		{
			RunScene(T::GetTypeID());
		}

		void BLAZE_API StopActiveScene();		

		SceneTypeID BLAZE_API GetActiveSceneTypeID();

		void BLAZE_API Update(float);
	}
}

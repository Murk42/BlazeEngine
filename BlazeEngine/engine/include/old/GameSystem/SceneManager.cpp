#include "SceneManager.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/DataStructures/Guard.h"
#include "BlazeEngine/Utilities/ThreadWorker.h"
#include "BlazeEngine/GameSystem/Resource.h"
#include "BlazeEngine/Internal/TypeStructs.h"

namespace Blaze
{
	extern std::vector<SceneType> sceneTypes;

	namespace SceneManager
	{		
		extern Guard<BaseScene*, true> scenes;
		extern BaseScene* activeScene;
		
		extern Worker<void, void*> loadingThread;
		extern BaseScene* loadingScene;
		extern Worker<void, void*> unloadingThread;
		extern BaseScene* unloadingScene;
		extern Worker<void, void*> savingThread;
		extern BaseScene* savingScene;				

		bool Setup()
		{			
			scenes = new BaseScene* [sceneTypes.size()];
			memset(scenes.ptr, 0, sizeof(BaseScene*) * sceneTypes.size());
			
			loadingScene = nullptr;
			unloadingScene = nullptr;
			savingScene = nullptr;
			return true;
		}

		void Cleanup()
		{
			if (activeScene != nullptr)
				StopActiveScene();

			if (loadingScene != nullptr)
			{
				loadingThread.WaitToFinish();
				scenes[loadingScene->typeID] = std::exchange(loadingScene, nullptr);
			}

			if (unloadingScene != nullptr)
			{				
				unloadingThread.WaitToFinish();

				const auto& type = sceneTypes[unloadingScene->typeID];
				void* ptr = type.ToDerived(unloadingScene);

				type.destruct(ptr);
				type.Deallocate(ptr);

				unloadingScene = nullptr;
			}

			if (savingScene != nullptr)
			{
				savingThread.WaitToFinish();
				savingScene = nullptr;
			}

			for (uint i = 0; i < sceneTypes.size(); ++i)
				if (scenes[i] != nullptr)
				{
					const auto& type = sceneTypes[i];
					void* ptr = type.ToDerived(scenes[i]);

					type.unload(ptr);
					type.destruct(ptr);
					type.Deallocate(ptr);
				}

			delete[] scenes.ptr;
			scenes.ptr = nullptr;
		}

		void LoadScene(SceneTypeID typeID)
		{
			if (scenes[typeID] == nullptr)
			{
				const auto& type = sceneTypes[typeID];
				void* ptr = type.Allocate();

				type.construct(ptr);
				type.load(ptr);

				scenes[typeID] = type.ToBase(ptr);
			}
		}

		void LoadSceneAsync(SceneTypeID typeID)
		{			
			if (scenes[typeID] == nullptr)
				if (loadingScene == nullptr)
				{
					const auto& type = sceneTypes[typeID];
					void* ptr = type.Allocate();

					type.construct(ptr);
					loadingScene = type.ToBase(ptr);

					loadingThread.AssignWork(sceneTypes[typeID].load, ptr);
				}
				else				
					Logger::AddLog(LogType::Error, __FUNCTION__,"Cant load two scenes async at once");				
		}

		void UnloadScene(SceneTypeID typeID)
		{
			if (scenes[typeID] != nullptr)			
				if (scenes[typeID] != activeScene)				
					sceneTypes[typeID].unload(sceneTypes[typeID].ToDerived(scenes[typeID]));				
				else				
					Logger::AddLog(LogType::Warning, __FUNCTION__, "You cant unload a running scene");							
			else			
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to unload a scene that isnt loaded");			
		}

		void UnloadSceneAsync(SceneTypeID typeID)
		{
			if (scenes[typeID] != nullptr)
				if (scenes[typeID] != activeScene)
					if (unloadingScene == nullptr)
					{
						unloadingScene = std::exchange(scenes[typeID], nullptr);
						unloadingThread.AssignWork(sceneTypes[typeID].unload, sceneTypes[typeID].ToDerived(unloadingScene));
					}
					else
						Logger::AddLog(LogType::Error, __FUNCTION__, "Cant unload two scenes async at once");
				else
					Logger::AddLog(LogType::Warning, __FUNCTION__, "You cant unload a running scene");
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to unload a scene that wasnt loaded");
		}

		void SaveScene(SceneTypeID typeID)
		{	
			if (scenes[typeID] != nullptr)			
				sceneTypes[typeID].save(sceneTypes[typeID].ToDerived(scenes[typeID]));			
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to save a scene that isnt loaded");
		}

		void SaveSceneAsync(SceneTypeID typeID)
		{
			if (scenes[typeID] != nullptr)
				if (savingScene == nullptr)
				{
					savingScene = scenes[typeID];
					savingThread.AssignWork(sceneTypes[typeID].save, sceneTypes[typeID].ToDerived(savingScene));
				}
				else
					Logger::AddLog(LogType::Warning, __FUNCTION__, "Cant save two scenes async at once");
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to save a scene that isnt loaded");
		}

		bool IsLoaded(SceneTypeID typeID)
		{
			return scenes[typeID] != nullptr;			
		}

		bool IsLoading(SceneTypeID typeID)
		{
			return loadingScene != nullptr;			
		}

		void RunScene(SceneTypeID typeID)
		{
			if (scenes[typeID] == nullptr)
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to run a scene that isnt loaded");
				return;
			}

			if (activeScene != nullptr)
				sceneTypes[activeScene->typeID].stop(sceneTypes[activeScene->typeID].ToDerived(activeScene));
			activeScene = sceneTypes[typeID].ToBase(scenes[typeID]);
			sceneTypes[typeID].start(sceneTypes[typeID].ToDerived(activeScene));
		}

		void StopActiveScene()
		{
			if (activeScene == nullptr)
			{
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to stop a scene, but no scene is running");
				return;
			}
			
			sceneTypes[activeScene->typeID].stop(sceneTypes[activeScene->typeID].ToDerived(activeScene));
			activeScene = nullptr;
		}

		SceneTypeID GetActiveSceneTypeID()
		{
			if (activeScene != nullptr)
				return activeScene->typeID;
			else
				return -1;
		}

		void Update(float dt)
		{
			for (uint i = 0; i < sceneTypes.size(); ++i)
			{
				if (loadingScene != nullptr && loadingThread.IsFinished())				
					scenes[i] = std::exchange(loadingScene, nullptr);		

				if (unloadingScene != nullptr && unloadingThread.IsFinished())
				{
					sceneTypes[unloadingScene->typeID].Deallocate(sceneTypes[unloadingScene->typeID].ToDerived(unloadingScene));					
					unloadingScene = nullptr;
				}

				if (savingScene != nullptr && savingThread.IsFinished())
				{
					savingScene = nullptr;
				}

				if (activeScene != nullptr)
					sceneTypes[activeScene->typeID].update(sceneTypes[activeScene->typeID].ToDerived(activeScene), dt);
			}
		}
	}
}
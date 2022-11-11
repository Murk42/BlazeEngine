#include "BlazeEngine/Utilities/Threading.h"
#include "source/BlazeEngine/Internal/EngineData.h"
#include <mutex>
#include <unordered_map>


namespace Blaze
{
	namespace Threading
	{
		static std::mutex threadIDMapMutex;		

		uint GetThreadID(std::thread::id id)
		{
			std::lock_guard<std::mutex> lk(threadIDMapMutex);

			auto it = engineData->threadIDMap.find(id);

			if (it == engineData->threadIDMap.end())
				return (engineData->threadIDMap[id] = engineData->threadIDMap.size());

			return engineData->threadIDMap[id];
		}
	}
}
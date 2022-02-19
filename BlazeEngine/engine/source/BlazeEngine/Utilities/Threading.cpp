#include "BlazeEngine/Utilities/Threading.h"
#include <mutex>
#include <unordered_map>


namespace Blaze
{
	namespace Threading
	{
		static std::mutex threadIDMapMutex;
		static std::unordered_map<std::thread::id, Blaze::uint> threadIDMap;

		uint GetThreadID(std::thread::id id)
		{
			std::lock_guard<std::mutex> lk(threadIDMapMutex);

			auto it = threadIDMap.find(id);

			if (it == threadIDMap.end())
				return (threadIDMap[id] = threadIDMap.size());

			return threadIDMap[id];
		}
	}
}
#include "pch.h"
#include "BlazeEngine/Utilities/Threading.h"

namespace Blaze
{
	namespace Threading
	{
		static std::mutex threadIDMapMutex;		

		uint GetThreadID(std::thread::id id)
		{
			std::lock_guard<std::mutex> lk(threadIDMapMutex);

			//auto it = engineData->threadIDMap.Find(id);
			auto [it, inserted] = engineData->threadIDMap.Insert(id, (uint)engineData->threadIDMap.Count());

			return it->value;				
		}
		uint GetCurrentThreadID()
		{
			return GetThreadID(std::this_thread::get_id());
		}
	}
}
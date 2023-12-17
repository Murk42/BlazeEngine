#include "pch.h"
#include "GlobalData.h"

namespace Blaze
{
	GlobalDataCore globalData;

	GlobalDataCore::GlobalDataCore()
	{		
	}
	GlobalDataCore::~GlobalDataCore()
	{

	}
	uint GetThisThreadID()
	{
		return globalData.threadIDs.Insert(std::this_thread::get_id(), (uint)globalData.threadIDs.Count()).iterator->value;		
	}
}
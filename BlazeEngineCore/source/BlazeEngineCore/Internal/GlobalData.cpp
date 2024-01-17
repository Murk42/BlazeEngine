#include "pch.h"
#include "GlobalData.h"

//Used in BlazeEngine library in EntryPoint.cpp as a empty function
void AddLoggerOutputFiles()
{

}

namespace Blaze
{
	GlobalDataCore globalDataCore;

	GlobalDataCore::GlobalDataCore()
	{		
	}
	GlobalDataCore::~GlobalDataCore()
	{

	}
	uint GetThisThreadID()
	{
		return globalDataCore.threadIDs.Insert(std::this_thread::get_id(), (uint)globalDataCore.threadIDs.Count()).iterator->value;		
	}
}
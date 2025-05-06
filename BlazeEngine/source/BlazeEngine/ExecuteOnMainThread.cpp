#include "pch.h"
#include "BlazeEngine/ExecuteOnMainThread.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"

namespace Blaze
{
	void ExecuteOnMainThread(void(*function)(void*), void* userData)
	{
		blazeEngineContext.ExecuteTaskOnMainThread(function, userData);
	}
}
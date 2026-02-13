#include "pch.h"
#include "BlazeEngine/Core/Threading/ThreadGroup.h"

namespace Blaze
{
	ThreadGroup::ThreadGroup()
	{
	}

	void ThreadGroup::AllocateThreads(uintMem threadCount)
	{
		threads.Resize(threadCount);
	}

	bool ThreadGroup::IsAnyRunning()
	{
		for (auto& thread : threads)
			if (thread.IsRunning())
				return true;
		return false;
	}

	uintMem ThreadGroup::WaitForAll(float timeout)
	{
		uintMem count = 0;
		for (auto& thread : threads)
			count += thread.WaitToFinish(timeout) ? 1 : 0;
		return count;
	}
}
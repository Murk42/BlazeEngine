#include "pch.h"
#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Core/Threading/ThreadPool.h"
#include "BlazeEngine/Core/Platform/Windows.h"

namespace Blaze
{
	struct TaskData
	{
		ThreadPool::TaskFunction function;
		void* userData;
	};

	struct ThreadPoolImpl
	{
		PTP_POOL pool = NULL;
		TP_CALLBACK_ENVIRON callbackEnvironment{ };
		PTP_CLEANUP_GROUP cleanupGroup = NULL;


		ThreadPoolImpl()
		{
			pool = CreateThreadpool(NULL);
			InitializeThreadpoolEnvironment(&callbackEnvironment);
			cleanupGroup = CreateThreadpoolCleanupGroup();

			SetThreadpoolCallbackPool(&callbackEnvironment, pool);
			SetThreadpoolCallbackCleanupGroup(&callbackEnvironment, cleanupGroup, NULL);
		}
		~ThreadPoolImpl()
		{
			CloseThreadpoolCleanupGroupMembers(cleanupGroup, TRUE, NULL);
			CloseThreadpoolCleanupGroup(cleanupGroup);
			DestroyThreadpoolEnvironment(&callbackEnvironment);
			CloseThreadpool(pool);
		}
	};

#undef THIS
#define THIS static_cast<ThreadPoolImpl*>(this->impl)

	static VOID CALLBACK SimpleCallback(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)
	{
		TaskData taskData = *static_cast<TaskData*>(Context);
		delete static_cast<TaskData*>(Context);

		taskData.function(taskData.userData);
	}	

	ThreadPool::ThreadPool()
		: impl(new ThreadPoolImpl())
	{		
	}
	ThreadPool::ThreadPool(ThreadPool&& other) noexcept
		: impl(std::exchange(other.impl, nullptr))
	{
	}
	ThreadPool::~ThreadPool()
	{
		Clear();
	}

	void ThreadPool::Clear()
	{
		if (impl == nullptr)
			return;

		delete impl;
	}
	void ThreadPool::SetThreadCountHint(uintMem minThreads, uintMem maxThreads)
	{
		if (impl == nullptr) impl = new ThreadPoolImpl();

		SetThreadpoolThreadMinimum(THIS->pool, static_cast<DWORD>(minThreads));
		SetThreadpoolThreadMaximum(THIS->pool, static_cast<DWORD>(maxThreads));
	}
	void ThreadPool::RunTask(TaskFunction function, void* userData)
	{
		if (impl == nullptr) impl = new ThreadPoolImpl();

		if (function == nullptr)
			return;
	
		if (TrySubmitThreadpoolCallback(SimpleCallback, new TaskData{ function, userData }, &THIS->callbackEnvironment) == FALSE)
		{
			BLAZE_WINDOWS_ERROR("TrySubmitThreadpoolCallback failed with error message: {}", Windows::GetErrorString(GetLastError()));
			return;
		}
	}
	ThreadPool& ThreadPool::operator=(ThreadPool&& other) noexcept
	{
		Clear();

		impl = std::exchange(other.impl, nullptr);

		return *this;
	}
	int ThreadPool::ThreadFunc(void* userData)
	{
		return 0;
	}
}
#endif
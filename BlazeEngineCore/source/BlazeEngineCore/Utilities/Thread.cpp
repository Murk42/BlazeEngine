#include "pch.h"
#include "BlazeEngineCore/Threading/Thread.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#include "BlazeEngineCore/Internal/Windows/WindowsPlatform.h"
#else
#error Not supported
#endif

static DWORD WINAPI std_ThreadProc(
	_In_ LPVOID lpParameter
)
{
	std::function<int()>* func = (std::function<int()>*)lpParameter;
	int ret = func->operator()();
	delete func;
	return ret;
}
static DWORD WINAPI static_ThreadProc(
	_In_ LPVOID lpParameter
)
{
	int(*func)() = (int(*)())lpParameter;
	return func();
}

namespace Blaze
{
	Thread::Thread()
		: handle(nullptr)
	{
	}
	Thread::Thread(std::function<int()> func)
	{		
		Run(func);
	}
	Thread::Thread(Thread&& t) noexcept
	{
		handle = t.handle;
		t.handle = nullptr;
	}
	Thread::~Thread()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle != nullptr)
			CloseHandle(handle);
#endif
	}

	Result Thread::Run(std::function<int()> func)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (IsRunning())		
			return BLAZE_ERROR_RESULT("Blaze Engine", "Thread::Run called on a running thread");

		std::function<int()>* ptr = new std::function<int()>(std::move(func));

		handle = CreateThread(NULL, 0, std_ThreadProc, ptr, 0, NULL);
#endif
		return Result();
	}

	Result Thread::Run(int(*func)())
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (IsRunning())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Thread::Run called on a running thread");		

		handle = CreateThread(NULL, 0, static_ThreadProc, func, 0, NULL);
#endif
		return Result();
	}

	void Thread::WaitToFinish() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS		
		WaitForSingleObject(handle, INFINITE); // wait infinitely
#endif
	}

	bool Thread::IsRunning() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle == NULL)
			return false;

		DWORD result = WaitForSingleObject(handle, 0);

		if (result == WAIT_OBJECT_0)
			return false;			
		else if (result == WAIT_TIMEOUT) 		
			return true;			
		else
		{
			Debug::Logger::LogError("WinAPI", "WaitForSingleObject failed with error \"" + Windows::GetErrorString(GetLastError()) + "\"");
			return false;
		}
#endif
	}	

	Thread& Thread::operator=(Thread&& t) noexcept
	{		
		handle = t.handle;
		t.handle = nullptr;
		return *this;
	}
}
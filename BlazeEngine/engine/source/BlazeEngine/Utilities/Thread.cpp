#include "BlazeEngine/Utilities/Thread.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#else
#error Not supported
#endif

static DWORD WINAPI ThreadProc(
	_In_ LPVOID lpParameter
)
{
	std::function<int()>* func = (std::function<int()>*)lpParameter;
	int ret = func->operator()();
	delete func;
	return ret;
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

	void Thread::Run(std::function<int()> func)
	{
		if (handle != nullptr)
			Logger::AddLog(Log(LogType::Fatal, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "Blaze Engine", "Thread::Run called on a running thread"));

		std::function<int()>* ptr = new std::function<int()>(std::move(func));

#ifdef BLAZE_PLATFORM_WINDOWS
		handle = CreateThread(NULL, 0, ThreadProc, ptr, 0, NULL);
#endif
	}

	void Thread::WaitToFinish()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		WaitForSingleObject(handle, INFINITE); // wait infinitely
#endif
	}

	void Thread::Terminate()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		TerminateThread(handle, 0);
#endif
		handle = nullptr;
	}


	Thread& Thread::operator=(Thread&& t) noexcept
	{
		if (handle != nullptr)
			Logger::AddLog(Log(LogType::Fatal, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "Blaze Engine", "Thread::operator= called on a running thread"));

		handle = t.handle;
		t.handle = nullptr;
		return *this;
	}
}
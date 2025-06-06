#include "pch.h"
#include "BlazeEngineCore/Threading/Thread.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/String.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngineCore/Internal/WindowsPlatform.h"
#include <aclapi.h>
#else
#error Not supported
#endif

namespace Blaze
{
	Thread::Thread()
		: handle(nullptr)
	{
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

	void Thread::Run(unsigned long(*func)(void*), void* userData)
	{
		RunImpl(func, userData);
	}

	bool Thread::WaitToFinish(float timeout) const
	{
#ifdef BLAZE_PLATFORM_WINDOWS				
		DWORD result = WaitForSingleObject(handle, timeout == FLT_MAX ? INFINITE : static_cast<DWORD>(timeout * 1000.0f));

		if (result == WAIT_OBJECT_0)
			return true;
		else if (result == WAIT_TIMEOUT)
			return false;
		else
		{
			Debug::Logger::LogError("WinAPI", "WaitForSingleObject failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return false;
		}
#endif
	}

	bool Thread::IsRunning() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle == NULL)
			return false;

		DWORD exitCode = 0;
		if (GetExitCodeThread(handle, &exitCode) == 0)
		{
			Debug::Logger::LogError("WinAPI", "GetExitCodeThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return false;
		}
		if (exitCode == STILL_ACTIVE)
			return true;
		else
			return false;		
#endif
	}
	bool Thread::GetExitCode(uint& code) const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle == NULL)
			return 0;

		DWORD exitCode = 0;
		if (GetExitCodeThread(handle, &exitCode) == 0)
		{
			Debug::Logger::LogError("WinAPI", "GetExitCodeThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return false;
		}
		if (exitCode == STILL_ACTIVE)
			return false;
		else
		{
			code = exitCode;
			return true;
		}
#endif
	}


	Thread& Thread::operator=(Thread&& t) noexcept
	{		
		handle = t.handle;
		t.handle = nullptr;
		return *this;
	}	
	void Thread::RunImpl(unsigned long(*function)(void*), void* data)
	{	
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle != NULL)
		{
			if (IsRunning())
			{
				BLAZE_ENGINE_CORE_ERROR("Thread::Run called on a running thread");
				return;
			}

			CloseHandle(handle);
		}
		
		handle = CreateThread(NULL, 0, function, data, 0, NULL);

		if (handle == NULL)
		{
			Debug::Logger::LogError("WinAPI", "CreateThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return;
		}
#endif					
	}	
}
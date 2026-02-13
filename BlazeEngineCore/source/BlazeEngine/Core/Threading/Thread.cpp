#include "pch.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/String/String.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Core/Internal/WindowsPlatform.h"
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
		Release();
	}
	void Thread::Release()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle != nullptr)
		{
			CloseHandle(handle);
			handle = nullptr;
		}
#endif
	}
	void Thread::SetThreadName(u8StringView name)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle == nullptr)
		{
			BLAZE_LOG_ERROR("Calling SetThreadName on a non-running thread");
			return;
		}

		u16String convertedString = name.ConvertToStringType<char16_t>();
		HRESULT result = SetThreadDescription(handle, (PCWSTR)convertedString.Ptr());

		if (FAILED(result))
		{
			BLAZE_WINDOWS_ERROR("SetThreadDescription failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return;
		}
#endif
	}
	u8String Thread::GetThreadName() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle == nullptr)
		{
			BLAZE_LOG_ERROR("Calling GetThreadName on a non-running thread");
			return { };
		}

		PWSTR description = NULL;
		HRESULT result = GetThreadDescription(handle, &description);

		if (FAILED(result))
		{
			BLAZE_WINDOWS_ERROR("GetThreadDescription failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return { };
		}

		uintMem descriptionLength = wcslen(description);

		return u16StringView(reinterpret_cast<char16_t*>(description), descriptionLength).ConvertToStringType<char8_t>();
#endif
	}
	bool Thread::Run(BasicThreadFunction function, void* userData)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (handle != NULL)
		{
			if (IsRunning())
			{
				BLAZE_LOG_ERROR("Run called on a running thread");
				return false;
			}

			CloseHandle(handle);
			handle = nullptr;
		}

		if (function == nullptr)
		{
			BLAZE_LOG_ERROR("Function pointer is nullptr");
			return false;
		}

		struct FunctionRelayData
		{
			std::atomic_flag dataCopied;
			Thread::BasicThreadFunction function;
			void* userData;
		};

		auto wrapperFunction = [](void* _relayData) -> unsigned /*__stdcall - isn't needed because non-capturing lambdas can implicitly be converted to such functions*/
		{
			FunctionRelayData* relayData = static_cast<FunctionRelayData*>(_relayData);

			Thread::BasicThreadFunction function = relayData->function;
			void* userData = relayData->userData;

			relayData->dataCopied.test_and_set();
			relayData->dataCopied.notify_all();

			_endthreadex(function(userData));
			return 0;
		};

		FunctionRelayData relayData{
			.dataCopied = { },
			.function = function,
			.userData = userData
		};

		handle = (void*)_beginthreadex(NULL, 0, wrapperFunction, &relayData, 0, &id);

		if (handle == NULL)
		{
			BLAZE_WINDOWS_ERROR("CreateThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return false;
		}

		relayData.dataCopied.wait(false);

		return true;		
#endif
	}
	bool Thread::WaitToFinish(float timeout) const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		DWORD result = WaitForSingleObject(handle, timeout == FLT_MAX ? INFINITE : static_cast<DWORD>(timeout * 1000.0f));

		if (result == WAIT_OBJECT_0)
		{
			return true;
		}
		else if (result == WAIT_TIMEOUT)
			return false;
		else
		{
			BLAZE_WINDOWS_ERROR("WaitForSingleObject failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
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
			BLAZE_WINDOWS_ERROR("GetExitCodeThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
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
			BLAZE_WINDOWS_ERROR("GetExitCodeThread failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
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
	void Thread::SetThreadPriority(ThreadPriority newPriority)
	{
		if (handle == nullptr)
		{
			BLAZE_LOG_ERROR("Calling SetThreadPriority on a non-running thread");
			return;
		}

		int _priority;
		switch (newPriority)
		{
		case ThreadPriority::Idle:         _priority = THREAD_PRIORITY_IDLE; break;
		case ThreadPriority::Lowest:       _priority = THREAD_PRIORITY_LOWEST; break;
		case ThreadPriority::Low:          _priority = THREAD_PRIORITY_BELOW_NORMAL; break;
		case ThreadPriority::Normal:       _priority = THREAD_PRIORITY_NORMAL; break;
		case ThreadPriority::High:         _priority = THREAD_PRIORITY_ABOVE_NORMAL; break;
		case ThreadPriority::Highest:      _priority = THREAD_PRIORITY_HIGHEST; break;
		case ThreadPriority::TimeCritical: _priority = THREAD_PRIORITY_TIME_CRITICAL; break;
		default:
			BLAZE_LOG_ERROR("Invalid ThreadPriority enum value");
			return;
		}

		if (::SetThreadPriority(handle, _priority) == 0)
			BLAZE_WINDOWS_ERROR("SetThreadPriority failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
	}
	ThreadPriority Thread::GetThreadPriority()
	{
		if (handle == nullptr)
		{
			BLAZE_LOG_ERROR("Calling GetThreadPriority on a non-running thread");
			return ThreadPriority::Normal;
		}

		int priority = ::GetThreadPriority(handle);

		switch (priority)
		{
		case THREAD_PRIORITY_IDLE:          return ThreadPriority::Idle;
		case THREAD_PRIORITY_LOWEST:        return ThreadPriority::Lowest;
		case THREAD_PRIORITY_BELOW_NORMAL:  return ThreadPriority::Low;
		case THREAD_PRIORITY_NORMAL:        return ThreadPriority::Normal;
		case THREAD_PRIORITY_ABOVE_NORMAL:  return ThreadPriority::High;
		case THREAD_PRIORITY_HIGHEST:       return ThreadPriority::Highest;
		case THREAD_PRIORITY_TIME_CRITICAL: return ThreadPriority::TimeCritical;
		case THREAD_PRIORITY_ERROR_RETURN:
			BLAZE_WINDOWS_ERROR("GetThreadPriority failed with error \"{}\"", Windows::GetErrorString(GetLastError()));
			return ThreadPriority::Normal;
			break;
		default:
			BLAZE_LOG_ERROR("GetThreadPriority returned an unexpected value");
			return ThreadPriority::Normal;
		}
	}
	void* Thread::ReleaseOwnership()
	{
		void* _handle = handle;
		handle = nullptr;
		return _handle;
	}
	Thread& Thread::operator=(Thread&& t) noexcept
	{
		handle = t.handle;
		t.handle = nullptr;
		return *this;
	}
}
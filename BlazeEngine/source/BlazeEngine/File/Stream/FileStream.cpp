#include "pch.h"
#include "BlazeEngine/File/Stream/FileStream.h"


#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Internal/Windows/WindowsPlatform.h"
#else
#pragma error
#endif

namespace Blaze
{
	FileStreamBase::FileStreamBase()
		: file(nullptr)
	{
	}
	FileStreamBase::FileStreamBase(void* file)
		: file(nullptr)
	{
		Open(file);
	}
	FileStreamBase::~FileStreamBase()
	{
		Close();
	}
	Result FileStreamBase::Open(void* file)
	{
		Close();

		this->file = file;

		return Result();
	}
	bool FileStreamBase::IsOpen() const
	{
		return file != nullptr;
	}
	Result FileStreamBase::Close()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return Result();

		BOOL result = CloseHandle(file);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "CloseHandle failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		file = nullptr;

		return Result();
#else
#pragma error
#endif
	}
	Result FileStreamBase::Flush()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		BOOL result = FlushFileBuffers(file);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "FlushFileBuffers failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		return Result();
#else
#pragma error
#endif
	}
	bool FileStreamBase::MovePosition(intMem offset)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		LARGE_INTEGER position;
		position.QuadPart = offset;
		BOOL result = SetFilePointerEx(file, position, NULL, FILE_CURRENT);

		if (result == 0)
		{
			Debug::Logger::LogError("Windows API", "SetFilePointerEx failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");
			return false;
		}

		return true;
#else
#pragma error
#endif
	}
	bool FileStreamBase::SetPosition(uintMem offset)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		LARGE_INTEGER position;
		position.QuadPart = offset;
		BOOL result = SetFilePointerEx(file, position, NULL, FILE_BEGIN);

		if (result == 0)
		{
			Debug::Logger::LogError("Windows API", "SetFilePointerEx failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");
			return false;
		}

		return true;
#else
#pragma error
#endif
	}
	bool FileStreamBase::SetPositionFromEnd(intMem offset)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		LARGE_INTEGER position;
		position.QuadPart = offset;
		BOOL result = SetFilePointerEx(file, position, NULL, FILE_END);

		if (result == 0)
		{
			Debug::Logger::LogError("Windows API", "SetFilePointerEx failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");
			return false;
		}

		return true;
#else
#pragma error
#endif
	}
	uintMem FileStreamBase::GetPosition() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		LARGE_INTEGER position;
		LARGE_INTEGER nullPosition;
		nullPosition.QuadPart = 0;
		BOOL result = SetFilePointerEx(file, nullPosition, &position, FILE_CURRENT);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "SetFilePointerEx failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		return (uintMem)position.QuadPart;
#else
#pragma error
#endif
	}
	uintMem FileStreamBase::GetSize() const
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "file is nullptr");

		LARGE_INTEGER size;
		BOOL result = GetFileSizeEx(file, &size);

		if (result == 0)
		{
			Debug::Logger::LogError("Windows API", "GetFileSizeEx failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

			return 0;
		}

		return size.QuadPart;
#else
#pragma error
#endif
	}

	FileWriteStream::FileWriteStream()
	{
	}
	FileWriteStream::FileWriteStream(void* file)
		: FileStreamBase(file)
	{
	}
	FileWriteStream::~FileWriteStream()
	{
	}
	uintMem FileWriteStream::Write(const void* ptr, uintMem byteCount)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		OVERLAPPED overlapped;
		DWORD bytesWritten;
		BOOL result = WriteFile(GetHandle(), ptr, static_cast<DWORD>(byteCount), &bytesWritten, &overlapped);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "WriteFile failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		return bytesWritten;
#else
#error
#endif
	}

	FileReadStream::FileReadStream()
	{
	}
	FileReadStream::FileReadStream(void* file)
		: FileStreamBase(file)
	{
	}
	FileReadStream::~FileReadStream()
	{
	}
	uintMem FileReadStream::Read(void* ptr, uintMem byteCount)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		OVERLAPPED overlapped;
		DWORD bytesRead;
		BOOL result = ReadFile(GetHandle(), ptr, static_cast<DWORD>(byteCount), &bytesRead, &overlapped);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "ReadFile failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		return bytesRead;
#else
#error
#endif
	}
}
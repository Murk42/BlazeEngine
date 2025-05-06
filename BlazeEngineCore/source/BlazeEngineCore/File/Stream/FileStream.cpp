#include "pch.h"
#include "BlazeEngineCore/File/Stream/FileStream.h"


#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngineCore/Internal/WindowsPlatform.h"
#else
#pragma error
#endif

namespace Blaze
{
	FileStreamBase::FileStreamBase()
		: file(nullptr)
	{
	}	
	FileStreamBase::FileStreamBase(FileStreamBase&& other) noexcept
		: StreamBase(std::move(other)), file(other.file)
	{		
		other.file = nullptr;
	}	
	FileStreamBase::~FileStreamBase()
	{
		Close();
	}
	void FileStreamBase::AcquireHandle(void* file)
	{
		Close();

		this->file = file;		
	}
	bool FileStreamBase::IsOpen() const
	{
		return file != nullptr;
	}
	void FileStreamBase::Close()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
			return;

		BOOL result = CloseHandle(file);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "CloseHandle failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		file = nullptr;		
#else
#pragma error
#endif
	}
	void FileStreamBase::Flush()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return;
		}

		BOOL result = FlushFileBuffers(file);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "FlushFileBuffers failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");		
#else
#pragma error
#endif
	}
	bool FileStreamBase::MovePosition(intMem offset)
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		if (file == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return false;
		}

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
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return false;
		}

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
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return false;
		}

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
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return 0;
		}

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
		{
			BLAZE_ENGINE_CORE_ERROR("file is nullptr");
			return 0;
		}

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

	FileStreamBase& FileStreamBase::operator=(FileStreamBase&& other) noexcept
	{
		Close();

		StreamBase::operator=(std::move(other));

		file = other.file;
		other.file = nullptr;

		return *this;
	}

	uintMem FileWriteStream::Write(const void* ptr, uintMem byteCount)
	{
#ifdef BLAZE_PLATFORM_WINDOWS		
		DWORD bytesWritten;
		BOOL result = WriteFile(GetHandle(), ptr, static_cast<DWORD>(byteCount), &bytesWritten, nullptr);

		if (result == 0)
		{
			Debug::Logger::LogError("Windows API", "WriteFile failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");
			return 0;
		}

		return bytesWritten;
#else
#error
#endif
	}
	FileWriteStream::FileWriteStream()
	{
	}
	FileWriteStream::FileWriteStream(FileWriteStream&& other) noexcept
		: WriteStream(std::move(other)), FileStreamBase(std::move(other))
	{		
	}	
	FileWriteStream::~FileWriteStream()
	{
	}
	FileWriteStream& FileWriteStream::operator=(FileWriteStream&& other) noexcept
	{
		WriteStream::operator=(std::move(other));
		return *this;
	}

	uintMem FileReadStream::Read(void* ptr, uintMem byteCount)
	{
#ifdef BLAZE_PLATFORM_WINDOWS		
		DWORD bytesRead;
		BOOL result = ReadFile(GetHandle(), ptr, static_cast<DWORD>(byteCount), &bytesRead, nullptr);

		if (result == 0)
			Debug::Logger::LogError("Windows API", "ReadFile failed with error :\"" + Windows::GetErrorString(GetLastError()) + "\"");

		return bytesRead;
#else
#error
#endif
	}
	FileReadStream::FileReadStream()
	{
	}
	FileReadStream::FileReadStream(FileReadStream&& other) noexcept
		: ReadStream(std::move(other)), FileStreamBase(std::move(other))
	{
	}	
	FileReadStream::~FileReadStream()
	{
	}
	FileReadStream& FileReadStream::operator=(FileReadStream&& other) noexcept
	{
		ReadStream::operator=(std::move(other));

		return *this;
	}

	FileStream::FileStream()
	{ 
	}
	FileStream::FileStream(FileStream&& other) noexcept
		: FileReadStream(std::move(other)), FileWriteStream(std::move(other)), FileStreamBase(std::move(other))
	{
	}
	FileStream& FileStream::operator=(FileStream&& other) noexcept
	{
		FileReadStream::operator=(std::move(other));
		FileWriteStream::operator=(std::move(other));
		FileStreamBase::operator=(std::move(other));

		return *this;
	}
}
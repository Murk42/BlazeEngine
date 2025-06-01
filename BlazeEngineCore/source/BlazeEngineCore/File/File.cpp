#include "pch.h"
#include "BlazeEngineCore/File/File.h"
#include "BlazeEngineCore/File/FileSystem.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/Debug/Logger.h"

#include <codecvt>

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#include "BlazeEngineCore/Internal/WindowsPlatform.h"
#undef CreateDirectory
#else
#error
#endif 

namespace Blaze
{
	File::File()  
	{
	}
	File::File(File&& other) noexcept
		: FileStream(std::move(other)), FileStreamBase(std::move(other))
	{ 
	}
	File::File(const Path& path, FileAccessPermission mode)
	{
		Open(path, mode);
	}
	File::File(const Path& path, FileAccessPermission mode, FileOpenParameters parameters)
	{
		Open(path, mode, parameters);
	}
	File::~File()
	{
	}


	void File::Open(const Path& path, FileAccessPermission mode)
	{
		FileOpenParameters parameters;
		if (mode == FileAccessPermission::Read)
		{
			parameters.openOption = FileOpenOptions::OpenExisting;	
			parameters.createSubdirectories = false;
		}		
		else if (mode == FileAccessPermission::Write)
		{
			parameters.openOption = FileOpenOptions::CreateAlways;
		}
		else if (mode == FileAccessPermission::ReadWrite)
		{
			parameters.openOption = FileOpenOptions::OpenAlways;
		}

		Open(path, mode, parameters);
	}

	void File::Open(const Path& path, FileAccessPermission mode, FileOpenParameters parameters)
	{
#ifdef BLAZE_PLATFORM_WINDOWS		
		DWORD desiredAccess = 0;
		DWORD creationDisposition = 0;
		DWORD flagsAndAttributes = 0;

		switch (mode)
		{
		case Blaze::FileAccessPermission::Read: desiredAccess = GENERIC_READ; break;
		case Blaze::FileAccessPermission::Write: desiredAccess = GENERIC_WRITE; break;
		case Blaze::FileAccessPermission::ReadWrite: desiredAccess = GENERIC_READ | GENERIC_WRITE; break;
		default:
			BLAZE_ENGINE_CORE_ERROR("Invalid FileAccessPermission enum value");
			return;
		}

		switch (parameters.openOption)
		{
		case FileOpenOptions::CreateAlways: creationDisposition = CREATE_ALWAYS; break;
		case FileOpenOptions::CreateNew: creationDisposition = CREATE_NEW; break;
		case FileOpenOptions::OpenAlways: creationDisposition = OPEN_ALWAYS; break;
		case FileOpenOptions::OpenExisting: creationDisposition = OPEN_EXISTING; break;
		case FileOpenOptions::TruncateExisting: creationDisposition = TRUNCATE_EXISTING; break;
		default:
			BLAZE_ENGINE_CORE_ERROR("Invalid FileOpenOption enum value");
			return;
		}

		switch (parameters.usageHint)
		{
		case FileUsageHint::Normal: break;
		case FileUsageHint::RandomAccess: flagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS; break;
		case FileUsageHint::Sequential: flagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN; break;
		default:
			BLAZE_ENGINE_CORE_ERROR("Invalid FileUsageHint enum value");
			return;
			break;
		}

		switch (parameters.locationHint)
		{
		case FileLifetimeOption::Normal: break;
		case FileLifetimeOption::Temporary: flagsAndAttributes |= FILE_ATTRIBUTE_TEMPORARY; break;
		default:
			BLAZE_ENGINE_CORE_ERROR("Invalid FileLifetimeOption enum value");
			return;
			break;
		}

		//auto pathString = path.GetString();

		if (parameters.createSubdirectories)
		{
			Path parentPath = path.ParentPath();

			if (!parentPath.Exists() && !parentPath.Empty())
				FileSystem::CreateDirectory(parentPath);

		}

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto string = path.GetString();		
		auto wstring = converter.from_bytes((char*)string.Buffer(), (char*)string.Buffer() + string.BufferSize());
		HANDLE handle = CreateFileW(wstring.data(), desiredAccess, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creationDisposition, flagsAndAttributes, NULL);

		if (handle == INVALID_HANDLE_VALUE)
		{
			Debug::Logger::LogError("Windows API", "CreateFileA given a path \"{}\" failed with error: \"{}\"", path, Windows::GetErrorString(GetLastError()));
			return;
		}

		AcquireHandle(handle);
#else
#error
#endif		
	}
	File& File::operator=(File&& other) noexcept
	{
		FileStream::operator=(std::move(other));

		return *this;
	}
}
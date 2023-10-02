#pragma once
#include "BlazeEngine/File/File.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "source/BlazeEngine/Internal/Windows/WindowsPlatform.h"
#else
#error
#endif

namespace Blaze
{
	File::File()
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


	Result File::Open(const Path& path, FileAccessPermission mode)
	{
		FileOpenParameters parameters;
		if (mode == FileAccessPermission::Read)
		{
			parameters.openOption = FileOpenOptions::OpenExisting;	
			parameters.createSubdirectories = false;
		}		
		else if (mode == FileAccessPermission::Write)
		{
			parameters.openOption = FileOpenOptions::OpenAlways;			
		}
		else if (mode == FileAccessPermission::ReadWrite)
		{
			parameters.openOption = FileOpenOptions::OpenAlways;
		}

		return Open(path, mode, parameters);
	}

	Result File::Open(const Path& path, FileAccessPermission mode, FileOpenParameters parameters)
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
			Debug::Logger::LogError("Blaze Engine", "Invalid FileAccessPermission enum value");
			break;
		}

		switch (parameters.openOption)
		{
		case FileOpenOptions::CreateAlways: creationDisposition = CREATE_ALWAYS; break;
		case FileOpenOptions::CreateNew: creationDisposition = CREATE_NEW; break;
		case FileOpenOptions::OpenAlways: creationDisposition = OPEN_ALWAYS; break;
		case FileOpenOptions::OpenExisting: creationDisposition = OPEN_EXISTING; break;
		case FileOpenOptions::TruncateExisting: creationDisposition = TRUNCATE_EXISTING; break;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid FileOpenOption enum value");
			break;
		}

		switch (parameters.usageHint)
		{
		case FileUsageHint::Normal: break;
		case FileUsageHint::RandomAccess: flagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
		case FileUsageHint::Sequential: flagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid FileUsageHint enum value");
			break;
		}

		switch (parameters.locationHint)
		{
		case FileLifetimeOption::Normal: break;		
		case FileLifetimeOption::Temporary: flagsAndAttributes |= FILE_ATTRIBUTE_TEMPORARY; break;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid FileLifetimeOption enum value");
			break;
		}
		
		if (parameters.createSubdirectories)
		{
			std::filesystem::path _path { path.GetString().Ptr() };
			_path = _path.parent_path();

			std::error_code ec;
			bool exists = std::filesystem::exists(_path, ec);

			if (ec) return BLAZE_ERROR_RESULT("Blaze Engine", (String)"std::filesystem::exists failed with error: \"" + ec.message().c_str() + "\"");
			
			if (!exists)
			{
				std::filesystem::create_directories(_path, ec);

				if (ec) return BLAZE_ERROR_RESULT("Blaze Engine", (String)"std::filesystem::create_directories failed with error: \"" + ec.message().c_str() + "\"");
			}
		}

		HANDLE handle = CreateFileA(path.GetString().Ptr(), desiredAccess, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creationDisposition, flagsAndAttributes, NULL);

		if (handle == INVALID_HANDLE_VALUE)
		{
			return
				BLAZE_INFO_RESULT("Blaze Engine", "Path was: \"" + path.GetString() + "\"") +
				BLAZE_ERROR_RESULT("Windows API", "CreateFileA failed with error: \"" + Windows::GetErrorString(GetLastError()) + "\"");
		}

		FileStreamBase::Open(handle);
#else
#error
#endif

		return Result();
	}
}
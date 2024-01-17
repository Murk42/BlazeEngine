#include "pch.h"
#include "BlazeEngineCore/File/File.h"
#include "BlazeEngineCore/File/FileSystem.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngineCore/Internal/Windows/WindowsPlatform.h"
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
		: FileStream(std::move(other))
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
			parameters.openOption = FileOpenOptions::CreateAlways;
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
		
		//auto pathString = path.GetString();

		if (parameters.createSubdirectories)
		{
			Path parentPath = path.ParentPath();			
								
			if (!parentPath.Exists() && !parentPath.Empty())			
				FileSystem::CreateDirectory(parentPath);								

		}
		
		auto wstring = path.GetUnderlyingObject().wstring();		
		HANDLE handle = CreateFileW(wstring.data(), desiredAccess, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creationDisposition, flagsAndAttributes, NULL);

		if (handle == INVALID_HANDLE_VALUE)		
			return	BLAZE_ERROR_RESULT("Windows API", "CreateFileA given a path \"" + path.ToString() + "\" failed with error: \"" + Windows::GetErrorString(GetLastError()) + "\"");		

		FileStreamBase::Open(handle);
#else
#error
#endif

		return Result();
	}
	File& File::operator=(File&& other) noexcept
	{
		FileStream::operator=(std::move(other));

		return *this;
	}
}
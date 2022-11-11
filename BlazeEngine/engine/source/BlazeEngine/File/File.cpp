#include "BlazeEngine/File/File.h"
#include "BlazeEngine/Logging/Logger.h"
#include <io.h>
#include <fcntl.h>

namespace Blaze
{	
		File::File()
			: fd(-1)
		{
		}
		File::File(const Path& path, FileOpenMode mode, FileOpenFlags flags, FilePermission perms)
		{
			Open(path, mode, flags);
		}
		File::~File()
		{
			if (fd != -1 && _close(fd) == -1)
				Logger::AddLog(BLAZE_ERROR_LOG("stdlib", "Failed to close file"));
		}

		Result File::Open(const Path& path, FileOpenMode mode, FileOpenFlags flags, FilePermission perms)
		{
			errno_t returned = _sopen_s(&fd, path.GetString().Ptr(), (int)mode | (int)flags | _O_BINARY, _SH_DENYNO, (int)perms);

			if (returned != 0)
				return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"stdlib", "_sopen_s failed with error: " + String::Convert(returned)), true);

			return Result();
		}
		int File::Open(const char* path, FileOpenMode mode, FileOpenFlags flags, FilePermission perms)
		{
			errno_t returned = _sopen_s(&fd, path, (int)mode | (int)flags | _O_BINARY, _SH_DENYNO, (int)perms);

			return returned;
		}
		Result File::Close()
		{
			if (_close(fd) == -1)
			{
				fd = -1;

				return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"stdlib", "_close failed."), true);				
			}
			fd = -1;

			return Result();
		}
		size_t File::Size()
		{
			return _filelength(fd);
		}
		size_t File::Read(Buffer& buffer)
		{
			return _read(fd, buffer.Ptr(), buffer.Size());
		}
		size_t File::Read(void* ptr, size_t readAmount)
		{
			return _read(fd, ptr, readAmount);
		}
		size_t File::Write(BufferView buffer)
		{
			return _write(fd, buffer.Ptr(), buffer.Size());
		}
		bool File::IsOpen() const
		{
			return fd >= 0;
		}	
}           
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Core/Logger.h"
#include <io.h>
#include <fcntl.h>

namespace Blaze
{	
		File::File()
			: fd(-1)
		{
		}
		File::File(StringView path, FileOpenMode mode, FileOpenFlags flags, FilePermission perms)
		{
			Open(path, mode, flags);
		}
		File::~File()
		{
			if (fd != -1 && _close(fd) == -1)			
					BLAZE_ERROR_LOG("stdlib", "Failed to close file");
		}

		int File::Open(StringView path, FileOpenMode mode, FileOpenFlags flags, FilePermission perms)
		{
			errno_t returned = _sopen_s(&fd, path.Ptr(), (int)mode | (int)flags | _O_BINARY, _SH_DENYNO, (int)perms);

			if (returned != 0)
			{
				BLAZE_ERROR_LOG("stdlib", "Failed to open file, error number: " + String::Convert(returned));
				return BLAZE_ERROR;
			}
			return BLAZE_OK;
		}
		int File::Close()
		{
			if (_close(fd) == -1)
			{
				fd = -1;
				BLAZE_ERROR_LOG("stdlib", "Failed to close file");
				return BLAZE_ERROR;
			}
			fd = -1;
			return BLAZE_OK;
		}
		size_t File::Size()
		{
			return _filelength(fd);
		}
		size_t File::Read(Buffer& buffer)
		{
			return _read(fd, buffer.Ptr(), buffer.Size());
		}		
		size_t File::Write(BufferView buffer)
		{
			return _write(fd, buffer.Ptr(), buffer.Size());
		}
		inline bool File::IsOpen() const
		{
			return fd >= 0;
		}	
}           
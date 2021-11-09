#include "BlazeEngine/Utilities/FileSystem.h"

#include "BlazeEngine/Core/Logger.h"

#include <filesystem>
#include <fstream>
#include <system_error>

namespace Blaze
{
    namespace FileSystem
    {        
        int ResolveError(std::error_code& ec)
        {
            if (ec)
            {
                BLAZE_ERROR_LOG(ec.category().name(), ec.message().c_str());
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }
        int CreateFile(StringView path)
        {
            std::ofstream fs(path.Ptr());
            return (fs.is_open() ? BLAZE_OK : BLAZE_ERROR);
        }
        int CreateFolder(StringView path)
        {
            std::error_code ec;
            std::filesystem::remove(path.Ptr(), ec);
            return ResolveError(ec);
        }
        int DeleteFile(StringView path)
        {
            std::error_code ec;
            std::filesystem::remove(path.Ptr(), ec);
            return ResolveError(ec);
        }
        int DeleteFolder(StringView path)
        {
            std::error_code ec;
            std::filesystem::remove_all(path.Ptr(), ec);
            return ResolveError(ec);
        }
        int Exists(StringView path)
        {
            std::error_code ec;
            std::filesystem::exists(path.Ptr(), ec);
            return ResolveError(ec);
        }
        int Copy(StringView dst, StringView src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.Ptr(), dst.Ptr(), (std::filesystem::copy_options)flags, ec);
            return ResolveError(ec);
        }
    }
}
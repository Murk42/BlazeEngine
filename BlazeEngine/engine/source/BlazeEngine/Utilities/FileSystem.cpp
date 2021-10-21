#include "BlazeEngine/Utilities/FileSystem.h"
#include <filesystem>
#include <fstream>
#include "source/BlazeEngine/Internal/ErrorHandeling.h"

namespace Blaze
{
    namespace FileSystem
    {
        int CreateFile(StringView path)
        {
            std::ofstream fs(path.Ptr());
            return (fs.is_open() ? BLAZE_OK : BLAZE_ERROR);
        }
        int CreateFolder(StringView path)
        {
            std::error_code ec;
            return ResolveError(std::filesystem::create_directories(path.Ptr(), ec), ec);
        }
        int DeleteFile(StringView path)
        {
            std::error_code ec;
            return ResolveError(std::filesystem::remove(path.Ptr(), ec), ec);
        }
        int DeleteFolder(StringView path)
        {
            std::error_code ec;
            return ResolveError(std::filesystem::remove_all(path.Ptr(), ec), ec);
        }
        int Exists(StringView path)
        {
            std::error_code ec;
            return ResolveError(std::filesystem::exists(path.Ptr(), ec), ec);
        }
        int Copy(StringView dst, StringView src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.Ptr(), dst.Ptr(), (std::filesystem::copy_options)flags, ec);
            return ResolveError(ec);
        }
    }
}
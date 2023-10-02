#include "pch.h"
#include "BlazeEngine/File/FileSystem.h"

namespace Blaze
{
    namespace FileSystem
    {                         
        Path GetCurrentPath()
        {
            return Path(StringView((const char*)std::filesystem::current_path().u8string().c_str()));
        }
        Result CreateDirectory(const Path& path)
        {
            std::error_code ec;            
            std::filesystem::create_directory(path.GetString().Ptr(), ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::create_directory failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str());                
            }
            return Result();
        }
        Result DeleteFile(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(path.GetString().Ptr(), ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::remove failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str());                
            }
            return Result();
        }
        Result ResizeFile(const Path& path, size_t newSize)
        {
            std::error_code ec;
            std::filesystem::resize_file(path.GetString().Ptr(), newSize, ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::resize_file failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str());                
            }
            return Result();
        }
        Result FileSize(const Path& path, size_t& size)
        {
            std::error_code ec;
            size = std::filesystem::file_size(path.GetString().Ptr(), ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                       "std::filesystem::file_size failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str());                
            }
            return Result();
        }        

        Result DeleteDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove_all(path.GetString().Ptr(), ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::remove_all failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str());                
            }
            return Result();
        }   

        ResultValue<bool> Exists(const Path& path)
        {
            std::error_code ec;

            bool exists = std::filesystem::exists(path.GetString().Ptr(), ec);

            if (ec)
            {
                return { exists, BLAZE_ERROR_RESULT("Blaze Engine", 
                    "std::filesystem::exists failed with path: \"" + path.GetString() + "\". \n" + ec.message().c_str()) };                
            }
            
            return { exists, Result()};
        }
        Result CopyContents(const Path& dst, const Path& src)
        {
            std::error_code ec;
            std::filesystem::copy_file(src.GetString().Ptr(), dst.GetString().Ptr(), ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::copy_file failed with destination: \"" + dst.GetString() + "\" and source: \"" + src.GetString() + "\". \n" + ec.message().c_str());
            }
            return Result();
        }
        Result Copy(const Path& dst, const Path& src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.GetString().Ptr(), dst.GetString().Ptr(), (std::filesystem::copy_options)flags, ec);
            if (ec)
            {
                return BLAZE_ERROR_RESULT("Blaze Engine",
                    "std::filesystem::copy failed with destination: \"" + dst.GetString() + "\" and source: \"" + src.GetString() + "\". \n" + ec.message().c_str());
            }
            return Result();
        }
    }
}
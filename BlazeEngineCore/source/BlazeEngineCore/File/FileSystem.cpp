#include "pch.h"
#include "BlazeEngineCore/File/FileSystem.h"

namespace Blaze
{
    namespace FileSystem
    {                         
        Path GetCurrentPath()
        {
            return Path(std::filesystem::current_path());
        }
        Result CreateDirectory(const Path& path)
        {   
            std::error_code ec;                  
            std::filesystem::create_directory(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::create_directory given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
        Result CreateDirectoryRecursive(const Path& path)
        {
            std::error_code ec;
            std::filesystem::create_directories(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::create_directory given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
        Result DeleteFile(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::remove given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
        Result ResizeFile(const Path& path, uintMem newSize)
        {
            std::error_code ec;
            std::filesystem::resize_file(path.GetUnderlyingObject(), newSize, ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::resize_file given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
        uintMem FileSize(const Path& path)
        {
            std::error_code ec;
            uintMem size = std::filesystem::file_size(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                Debug::Logger::LogError("Blaze Engine", "std::filesystem::file_size given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return 0;
            }
            return size;
        }        

        Result DeleteDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove_all(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::remove_all given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }   
        
        Result CopyContents(const Path& dst, const Path& src)
        {
            std::error_code ec;            
            std::filesystem::copy_file(src.GetUnderlyingObject(), dst.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::copy_file given path \"" + src.ToString() + "\" as source and \"" + dst.ToString() + "\" as destination returned a error : \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
        Result Copy(const Path& dst, const Path& src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.GetUnderlyingObject(), dst.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                return BLAZE_ERROR_RESULT("Blaze Engine", "std::filesystem::copy given path \"" + src.ToString() + "\" as source and \"" + dst.ToString() + "\" as destination returned a error : \"" + StringView(message.data(), message.size()) + "\"");
            }
            return Result();
        }
    }
}
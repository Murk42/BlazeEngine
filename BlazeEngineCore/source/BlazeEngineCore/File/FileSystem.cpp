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
        void CreateDirectory(const Path& path)
        {   
            std::error_code ec;                  
            std::filesystem::create_directory(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::create_directory given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void CreateDirectoryRecursive(const Path& path)
        {
            std::error_code ec;
            std::filesystem::create_directories(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::create_directory given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void DeleteFile(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::remove given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }            
        }
        void ResizeFile(const Path& path, uintMem newSize)
        {
            std::error_code ec;
            std::filesystem::resize_file(path.GetUnderlyingObject(), newSize, ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::resize_file given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }            
        }
        uintMem FileSize(const Path& path)
        {
            std::error_code ec;
            uintMem size = std::filesystem::file_size(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::file_size given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return 0;
            }
            return size;
        }        

        void DeleteDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove_all(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::remove_all given an path \"" + path.ToString() + "\" returned a error: \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }            
        }   
        
        void CopyContents(const Path& dst, const Path& src)
        {
            std::error_code ec;            
            std::filesystem::copy_file(src.GetUnderlyingObject(), dst.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::copy_file given path \"" + src.ToString() + "\" as source and \"" + dst.ToString() + "\" as destination returned a error : \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void Copy(const Path& dst, const Path& src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.GetUnderlyingObject(), dst.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_ENGINE_CORE_ERROR("std::filesystem::copy given path \"" + src.ToString() + "\" as source and \"" + dst.ToString() + "\" as destination returned a error : \"" + StringView(message.data(), message.size()) + "\"");
                return;
            }            
        }
        TimePoint GetLastWriteTime(const Path& path)
        {
            std::error_code ec;
            auto tp = std::filesystem::last_write_time(path.GetUnderlyingObject(), ec);
            if (ec)
            {
                auto message = ec.message();
                Debug::Log(Debug::LogType::Error, "Blaze Engine", "std::filesystem::last_write_time given path \"" + path.ToString() + "\" returned a error : \"" + StringView(message.data(), message.size()) + "\"");
            }
                
            double seconds = (tp.time_since_epoch()).count() * 1e-8;            

            return TimePoint() + TimeInterval::FromSeconds(seconds);;
        }
        void SetLastWriteTime(const Path& path, const TimePoint& timePoint)
        {
            std::error_code ec;
            std::chrono::time_point<std::chrono::file_clock> clock{ std::chrono::microseconds( int64((timePoint - TimePoint()).ToSeconds() * 1e6)) };
            std::filesystem::last_write_time(path.GetUnderlyingObject(), clock, ec);
            if (ec)
            {
                auto message = ec.message();
                Debug::Log(Debug::LogType::Error, "Blaze Engine", "std::filesystem::last_write_time given path \"" + path.ToString() + "\" returned a error : \"" + StringView(message.data(), message.size()) + "\"");
            }            
        }
    }
}
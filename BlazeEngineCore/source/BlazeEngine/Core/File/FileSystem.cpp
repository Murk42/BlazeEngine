#include "pch.h"
#include "BlazeEngine/Core/File/FileSystem.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/Time/TimeInterval.h"
#include <filesystem>

namespace Blaze
{
    static Path ToPath(const std::filesystem::path& path)
    {
        auto string = path.u8string();
        return u8String(string.data(), string.size());
    }
    static std::filesystem::path ToPath(const Path& path)
    {
        auto string = path.GetString();
        return std::filesystem::path(std::u8string_view((char8_t*)string.Ptr(), string.Count()));
    }

    namespace FileSystem
    {
        Path GetCurrentPath()
        {
            return ToPath(std::filesystem::current_path());
        }
        void CreateDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::create_directory(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::create_directory given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void CreateDirectoryRecursive(const Path& path)
        {
            std::error_code ec;
            std::filesystem::create_directories(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::create_directory given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void DeleteFile(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::remove given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        void ResizeFile(const Path& path, uintMem newSize)
        {
            std::error_code ec;
            std::filesystem::resize_file(ToPath(path), newSize, ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::resize_file given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return;
            }
        }
        uintMem FileSize(const Path& path)
        {
            std::error_code ec;
            uintMem size = std::filesystem::file_size(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::file_size given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return 0;
            }
            return size;
        }

        void DeleteDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove_all(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::remove_all given an path \"{}\" returned a error: \"{}\"", path.GetString(), StringView(message.data(), message.size()) + "\"");
                return;
            }
        }

        void CopyContents(const Path& dst, const Path& src)
        {
            std::error_code ec;
            std::filesystem::copy_file(ToPath(src), ToPath(dst), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::copy_file given path \"{}\" as source and \"{}\" as destination returned a error : \"{}\"", src, dst, StringView(message.data(), message.size()));
                return;
            }
        }
        void Copy(const Path& dst, const Path& src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(ToPath(src), ToPath(dst), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::copy given path \"{}\" as source and \"{}\" as destination returned a error : \"{}\"", src, dst, StringView(message.data(), message.size()));
                return;
            }
        }
        ProgramTimePoint GetLastWriteTime(const Path& path)
        {
            std::error_code ec;
            auto tp = std::filesystem::last_write_time(ToPath(path), ec);
            if (ec)
            {
                auto message = ec.message();
                BLAZE_LOG_ERROR("std::filesystem::last_write_time given path \"{}\" returned a error: \"{}\"", path, StringView(message.data(), message.size()));
            }

            //TODO finish implementation
            return { };
        }
        void SetLastWriteTime(const Path& path, const ProgramTimePoint& timePoint)
        {
            //TODO finish implementation
            //std::error_code ec;
            //std::chrono::time_point<std::chrono::file_clock> clock{ std::chrono::microseconds( int64((timePoint - ProgramTimePoint()).GetSeconds() * 1e6)) };
            //std::filesystem::last_write_time(ToPath(path), clock, ec);
            //if (ec)
            //{
            //    auto message = ec.message();
            //    BLAZE_LOG_DEBUG("std::filesystem::last_write_time given path \"{}\" returned a error: \"{}\"", path, StringView(message.data(), message.size()));
            //}
        }
    }
}
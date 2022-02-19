#include "BlazeEngine/File/FileSystem.h"

#include "BlazeEngine/Logger/Logger.h"

#include <filesystem>
#include <fstream>
#include <system_error>

namespace Blaze
{
    namespace FileSystem
    {                         
        Path GetCurrentPath()
        {
            return Path(StringView(std::filesystem::current_path().u8string().c_str()));
        }
        int CreateFile(const Path& path)
        {
            std::ofstream fs(path.GetString().Ptr());
            if (!fs.is_open())
            {                
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to create file with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;            
        }
        int CreateDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(path.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to create directory with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK; 
        }
        int DeleteFile(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove(path.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to delete a fil with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }
        int ResizeFile(const Path& path, size_t newSize)
        {
            std::error_code ec;
            std::filesystem::resize_file(path.GetString().Ptr(), newSize, ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to resize a file with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }
        int FileSize(const Path& path, size_t& size)
        {
            std::error_code ec;
            size = std::filesystem::file_size(path.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to retrieve size of a file with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }        

        int DeleteDirectory(const Path& path)
        {
            std::error_code ec;
            std::filesystem::remove_all(path.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to delete a directory with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }   

        int Exists(const Path& path)
        {
            std::error_code ec;
            std::filesystem::exists(path.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Exists failed with path: \"" + path.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }
        int CopyContents(const Path& dst, const Path& src)
        {
            std::error_code ec;
            std::filesystem::copy_file(src.GetString().Ptr(), dst.GetString().Ptr(), ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to copy contents from \"" + src.GetString() + "\" to \"" + dst.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;
        }
        int Copy(const Path& dst, const Path& src, CopyFlags flags)
        {
            std::error_code ec;
            std::filesystem::copy(src.GetString().Ptr(), dst.GetString().Ptr(), (std::filesystem::copy_options)flags, ec);
            if (ec)
            {
                BLAZE_WARNING_LOG(ec.category().name(), ec.message().c_str());
                BLAZE_WARNING_LOG("Blaze Engine", "Failed to copy \"" + src.GetString() + "\" to \"" + dst.GetString() + "\".");
                return BLAZE_ERROR;
            }
            return BLAZE_OK;            
        }
    }
}
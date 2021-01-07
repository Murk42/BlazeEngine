#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include <cstdio>

namespace Blaze
{
    namespace File
    {
        bool Read(StringView path, ByteStream* ptr, bool emitWarning)
        {
            if (path.Size() == 0) return false;

            if (ptr == nullptr)
                return false;
            FILE* file = fopen(path.Ptr(), "rb");

            if (file != nullptr)
            {
                fseek(file, 0, SEEK_END);
                long size = ftell(file);
                fseek(file, 0, SEEK_SET);

                ptr->Resize(size);

                fread(ptr->GetData().data(), 1, size, file);

                fclose(file);
                return true;
            }
            else if (emitWarning)
                Logger::AddLog(LogType::Warning, __FUNCTION__, "Failed to read file with path \"" + String(path) + "\"");
            return false;
        }
        bool Write(StringView path, bool clear, ByteStream* ptr, bool emitWarning)
        {
            if (path.Size() == 0) return false;

            if (Exists(path))
            {
                FILE* file = fopen(path.Ptr(), (clear ? "wb" : "ab"));

                if (file != nullptr)
                {
                    if (ptr != nullptr)
                        fwrite(ptr->GetData().data(), 1, ptr->GetSize(), file);

                    fclose(file);
                    return true;
                }
            }
            else if (emitWarning)
                Logger::AddLog(LogType::Warning, __FUNCTION__, "Failed to write to file with path \"" + String(path) + "\". The file doesnt exist");
            return false;
        }
        bool Create(StringView path, ByteStream* ptr)
        {
            if (path.Size() == 0) return false;
            FILE* file = fopen(path.Ptr(), "w+bx");

            if (file != nullptr)
            {
                if (ptr != nullptr)
                    fwrite(ptr->GetData().data(), 1, ptr->GetSize(), file);

                fclose(file);
                return true;
            }
            return false;
        }
        bool Delete(StringView path)
        {
            if (path.Size() == 0) return false;
            return !((bool)remove(path.Ptr()));
        }
        bool Exists(StringView path)
        {
            if (path.Size() == 0) return false;
            FILE* file = fopen(path.Ptr(), "rb");
            if (file != nullptr)
            {
                fclose(file);
                return true;
            }
            return false;
        }
    }
}           
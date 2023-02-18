#include "Command.h"
#include <iostream>
#include <array>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITSTATUS
#endif

Result RunCommand(std::string command, std::string& result)
{
    int exitcode = 0;
    std::array<char, 128> buffer{};
    result = std::string();
    FILE* pipe = popen((command).c_str(), "r");
    if (pipe == nullptr) {
        throw std::runtime_error("popen() failed!");
    }
    try {
        std::size_t bytesread;
        while ((bytesread = std::fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
            result += std::string(buffer.data(), bytesread);
        }
    }
    catch (...) {
        pclose(pipe);
        throw;
    }

    exitcode = WEXITSTATUS(pclose(pipe));

    if (exitcode != 0)
        return Result("Failed to run command: \"" + command + "\"");

    return Result();
}
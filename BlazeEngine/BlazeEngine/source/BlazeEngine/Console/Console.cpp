#include "pch.h"
#include "BlazeEngine/Console/Console.h"
#include <string>
#include <codecvt>
#include <iostream>

namespace Blaze
{
    namespace Console
    {
        template<>
        void Write<>(u8StringView text)
        {
            std::cout << std::string_view(reinterpret_cast<const char*>(text.Ptr()), text.Count());
        }

        u8String Read()
        {
            std::string input;
            std::getline(std::cin, input);
            return u8String(input.data(), input.size());
        }
    }
}


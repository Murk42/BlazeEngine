#pragma once
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include <functional>

namespace std
{
    template <>
    struct BLAZE_API  hash<Blaze::String>
    {
        std::size_t operator()(const Blaze::String& k) const;
    };
    template <>
    struct BLAZE_API  hash<Blaze::StringView>
    {
        std::size_t operator()(const Blaze::StringView& k) const;
    };
    template <>
    struct BLAZE_API  hash<Blaze::StringUTF8>
    {
        std::size_t operator()(const Blaze::StringUTF8& k) const;
    };
    template <>
    struct BLAZE_API  hash<Blaze::StringViewUTF8>
    {
        std::size_t operator()(const Blaze::StringViewUTF8& k) const;
    };
}
#pragma once
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include <functional>

namespace std
{
    template <>
    struct hash<Blaze::String>
    {
        std::size_t operator()(const Blaze::String& k) const
        {                        
            return hash<string_view>()(std::string_view(k.Ptr(), k.Size()));
        }
    };
    template <>
    struct hash<Blaze::StringView>
    {
        std::size_t operator()(const Blaze::StringView& k) const
        {
            return hash<string_view>()(std::string_view(k.Ptr(), k.Size()));
        }
    };
    template <>
    struct hash<Blaze::StringUTF8>
    {
        std::size_t operator()(const Blaze::StringUTF8& k) const
        {
            return hash<string_view>()(std::string_view((const char*)k.Buffer(), k.BufferSize()));
        }
    };
    template <>
    struct hash<Blaze::StringViewUTF8>
    {
        std::size_t operator()(const Blaze::StringViewUTF8& k) const
        {
            return hash<string_view>()(std::string_view((const char*)k.Buffer(), k.BufferSize()));
        }
    };
}
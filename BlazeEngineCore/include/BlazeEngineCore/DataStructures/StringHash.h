#pragma once
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"

namespace std
{
    template <>
    struct hash<Blaze::String>
    {
        inline std::size_t operator()(const Blaze::String& k) const;
    };
    template <>
    struct hash<Blaze::StringView>
    {
        inline std::size_t operator()(const Blaze::StringView& k) const;
    };
    template <>
    struct hash<Blaze::StringUTF8>
    {
        inline std::size_t operator()(const Blaze::StringUTF8& k) const;
    };
    template <>
    struct hash<Blaze::StringViewUTF8>
    {
        inline std::size_t operator()(const Blaze::StringViewUTF8& k) const;
    };

    inline std::size_t hash<Blaze::String>::operator()(const Blaze::String& k) const
    {
        return hash<string_view>()(std::string_view(k.Ptr(), k.Count()));
    }

    inline std::size_t hash<Blaze::StringView>::operator()(const Blaze::StringView& k) const
    {
        return hash<string_view>()(std::string_view(k.Ptr(), k.Count()));
    }

    inline std::size_t hash<Blaze::StringUTF8>::operator()(const Blaze::StringUTF8& k) const
    {
        return hash<u8string_view>()(std::u8string_view((const char8_t*)k.Buffer(), k.BufferSize() - 1));
    }
    inline std::size_t hash< Blaze::StringViewUTF8>::operator()(const Blaze::StringViewUTF8& k) const
    {
        return hash<u8string_view>()(std::u8string_view((const char8_t*)k.Buffer(), k.BufferSize() - 1));
    }
}
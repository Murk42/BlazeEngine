#pragma once
#include "BlazeEngine/DataStructures/StringHash.h"
#include <string>

namespace std
{
    std::size_t hash<Blaze::String>::operator()(const Blaze::String& k) const
    {
        return hash<string_view>()(std::string_view(k.Ptr(), k.Size()));
    }
        
    std::size_t hash<Blaze::StringView>::operator()(const Blaze::StringView& k) const
    {
        return hash<string_view>()(std::string_view(k.Ptr(), k.Size()));
    }
    
    std::size_t hash<Blaze::StringUTF8>::operator()(const Blaze::StringUTF8& k) const
    {
        return hash<string_view>()(std::string_view((const char*)k.Buffer(), k.BufferSize()));
    }
    std::size_t hash< Blaze::StringViewUTF8>::operator()(const Blaze::StringViewUTF8& k) const
    {
        return hash<string_view>()(std::string_view((const char*)k.Buffer(), k.BufferSize()));
    }
}
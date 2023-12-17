#pragma once
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"

namespace std
{
    template <>
    struct BLAZE_CORE_API  hash<Blaze::String>
    {
        std::size_t operator()(const Blaze::String& k) const;
    };
    template <>
    struct BLAZE_CORE_API  hash<Blaze::StringView>
    {
        std::size_t operator()(const Blaze::StringView& k) const;
    };
    template <>
    struct BLAZE_CORE_API  hash<Blaze::StringUTF8>
    {
        std::size_t operator()(const Blaze::StringUTF8& k) const;
    };
    template <>
    struct BLAZE_CORE_API  hash<Blaze::StringViewUTF8>
    {
        std::size_t operator()(const Blaze::StringViewUTF8& k) const;
    };
}

namespace Blaze
{
    inline uint32 StringView::Hash() const
    {
        return static_cast<uint32>(std::hash<StringView>()(*this));
    }
}
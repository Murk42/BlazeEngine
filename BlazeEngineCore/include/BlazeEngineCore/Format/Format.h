#pragma once
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/File/Path.h"
#include <format>

namespace std
{
	template<>
	struct formatter<Blaze::String> : formatter<string_view>
	{
		template<class FmtContext>
		FmtContext::iterator format(const Blaze::String& s, FmtContext& ctx) const
		{
			return formatter<string_view>::format<FmtContext>(string_view(s.Ptr(), s.Count()), ctx);
		}
	};
	template<>
	struct formatter<Blaze::StringUTF8> : formatter<string_view>
	{
		template<class FmtContext>
		FmtContext::iterator format(const Blaze::StringUTF8& s, FmtContext& ctx) const
		{
			return formatter<string_view>::format<FmtContext>(string_view((const char*)s.Buffer(), s.BufferSize()), ctx);
		}
	};
	template<>
	struct formatter<Blaze::StringView> : formatter<string_view>
	{
		template<class FmtContext>
		FmtContext::iterator format(const Blaze::StringView& s, FmtContext& ctx) const
		{
			return formatter<string_view>::format<FmtContext>(string_view(s.Ptr(), s.Count()), ctx);
		}
	};
	template<>
	struct std::formatter<Blaze::StringViewUTF8> : std::formatter<std::string_view>
	{
		template<class FmtContext>
		FmtContext::iterator format(const Blaze::StringViewUTF8& s, FmtContext& ctx) const
		{
			return std::formatter<std::string_view>::format<FmtContext>(std::string_view((const char*)s.Buffer(), s.BufferSize()), ctx);
		}
	};
	template<>
	struct std::formatter<Blaze::Path> : std::formatter<std::string_view>
	{
		template<class FmtContext>
		FmtContext::iterator format(const Blaze::Path& s, FmtContext& ctx) const
		{
			auto string = s.GetString();
			return std::formatter<std::string_view>::format<FmtContext>(std::string_view((char*)string.Buffer(), string.BufferSize()), ctx);
		}
	};
}
namespace Blaze
{
	template<typename T>
	concept Formatable = requires (const T & v, std::format_context ctx) {
		std::formatter<std::remove_cvref_t<T>>().format(v, ctx);
	};
	
	template<Formatable ... Args>
	String Format(const StringView& string, const Args& ... args)
	{
		std::string_view sv(string.Ptr(), string.Count());
		std::string out = std::vformat(sv, std::make_format_args(args...));
		return { out.data(), out.size() };
	}

	template<Formatable ... Args>
	StringUTF8 FormatUTF8(const StringViewUTF8& string, const Args& ... args)
	{
		std::string_view sv((const char*)string.Buffer(), string.BufferSize());
		std::string out = std::vformat(sv, std::make_format_args(args...));
		return { (void*)out.data(), out.size() };		
	}
}
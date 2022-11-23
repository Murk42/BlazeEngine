#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Array.h"

namespace Blaze::StringParsing
{
	bool BLAZE_API SplitAtFirst(StringView value, String& first, String& second, char ch);

	bool BLAZE_API GetAfterFirst(StringView value, String& out, char ch);
	bool BLAZE_API GetBeforeFirst(StringView value, String& out, char ch);

	bool BLAZE_API GetAfterLast(StringView value, String& out, char ch);
	bool BLAZE_API GetBeforeLast(StringView value, String& out, char ch);

	bool BLAZE_API RemoveQuotes(StringView value, String& out);
	bool BLAZE_API RemoveBrackets(StringView value, String& out, char right = '(', char left = ')');
	void BLAZE_API RemoveSpace(StringView value, String& out);

	Array<String> BLAZE_API Split(StringView value, char ch);

	bool BLAZE_API ConvertTo(const StringView& sv, uint64& value, uint base = 10);
}
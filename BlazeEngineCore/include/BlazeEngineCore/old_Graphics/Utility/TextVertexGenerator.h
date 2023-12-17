#pragma once
#include "BlazeEngineCore/Resources/Font/Font.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"

namespace Blaze::Graphics
{
	class BaseTextVertexGenerator
	{	
	protected:
		StringUTF8 text;
		FontResolution* fontResolution;

		uint rowCount;
		Vec2f cursor;
	public:
		virtual uint Setup(StringViewUTF8 text, FontResolution* fontResolution) = 0;
		virtual bool GetNextCharacterData(Vec2f& p1, Vec2f& p2) = 0;

		//TODO deprecated
		virtual bool IsEnd() const = 0;
		virtual bool HasMoreCharacters() const = 0;

		inline FontResolution* GetFontResolution() const { return fontResolution; }
		inline StringViewUTF8 GetString() const { return text; }
		inline uint GetRowCount() const { return rowCount; }
		inline Vec2f GetCursor() const { return cursor; }
	};

	class BLAZE_CORE_API DefaultTextVertexGenerator : public BaseTextVertexGenerator
	{			
		StringUTF8::Iterator it;				

		UnicodeChar prev;
		bool first;
	public:
		DefaultTextVertexGenerator();

		uint Setup(StringViewUTF8 text, FontResolution* fontResolution) override;
		bool GetNextCharacterData(Vec2f& p1, Vec2f& p2) override;

		bool IsEnd() const override;		

		bool HasMoreCharacters() const override;

		inline UnicodeChar GetCurrentCharacter() const { return it.ToUnicode(); }
	};	
}
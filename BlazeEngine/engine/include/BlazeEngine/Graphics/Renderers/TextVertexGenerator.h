#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"

namespace Blaze::Graphics
{
	class BLAZE_API BaseTextVertexGenerator
	{
	public:
		virtual void Setup(StringViewUTF8 text, FontResolution* fontResolution) = 0;
		virtual bool GenerateVertex(Vec2f& p1, Vec2f& p2, Vec2f& uv1, Vec2f& uv2) = 0;
		virtual bool IsEnd() const = 0;
		virtual uint GetMaxVertexCount() const = 0;
	};

	class BLAZE_API DefaultTextVertexGenerator : public BaseTextVertexGenerator
	{
		FontResolution* fontResolution;
		StringUTF8 text;		

		StringUTF8::Iterator it;
		Vec2f cursor;

		UnicodeChar prev;
		bool first;
	public:
		DefaultTextVertexGenerator();

		void Setup(StringViewUTF8 text, FontResolution* fontResolution) override;
		bool GenerateVertex(Vec2f& p1, Vec2f& p2, Vec2f& uv1, Vec2f& uv2) override;
		bool IsEnd() const override;

		uint GetMaxVertexCount() const override;		

		inline UnicodeChar GetCurrentCharacter() const { return it.ToUnicode(); }
	};
}